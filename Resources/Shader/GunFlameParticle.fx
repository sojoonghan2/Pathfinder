#ifndef _GUNFLAMEPARTICLE_FX_
#define _GUNFLAMEPARTICLE_FX_

#include "params.fx"
#include "utils.fx"

#define PI 3.141592

struct Particle
{
	float3 worldPos;
	float curTime;
	float3 worldDir;
	float lifeTime;
	int alive;
	float3 padding;
};

StructuredBuffer<Particle> g_data : register(t9);

struct VS_IN
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	uint id : SV_InstanceID;
};

struct VS_OUT
{
	float4 viewPos : POSITION;
	float2 uv : TEXCOORD;
	float id : ID;
};

VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT) 0.f;

	float3 worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;
	worldPos += g_data[input.id].worldPos;

	output.viewPos = mul(float4(worldPos, 1.f), g_matView);
	output.uv = input.uv;
	output.id = input.id;

	return output;
}

struct GS_OUT
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD;
	uint id : SV_InstanceID;
};

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
	GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };

	VS_OUT vtx = input[0];
	uint id = (uint) vtx.id;
	if (0 == g_data[id].alive)
		return;

	float ratio = g_data[id].curTime / g_data[id].lifeTime;
	float scale = ((g_float_1 - g_float_0) * ratio + g_float_0); // 확대 효과 위해 *1.0f

	output[0].position = vtx.viewPos + float4(-scale, scale, 0.f, 0.f);
	output[1].position = vtx.viewPos + float4(scale, scale, 0.f, 0.f);
	output[2].position = vtx.viewPos + float4(scale, -scale, 0.f, 0.f);
	output[3].position = vtx.viewPos + float4(-scale, -scale, 0.f, 0.f);

	for (int i = 0; i < 4; ++i)
		output[i].position = mul(output[i].position, g_matProjection);

	output[0].uv = float2(0.f, 0.f);
	output[1].uv = float2(1.f, 0.f);
	output[2].uv = float2(1.f, 1.f);
	output[3].uv = float2(0.f, 1.f);

	for (int i = 0; i < 4; ++i)
		output[i].id = id;

	outputStream.Append(output[0]);
	outputStream.Append(output[1]);
	outputStream.Append(output[2]);
	outputStream.RestartStrip();
	outputStream.Append(output[0]);
	outputStream.Append(output[2]);
	outputStream.Append(output[3]);
	outputStream.RestartStrip();
}

float4 PS_Main(GS_OUT input) : SV_Target
{
	float4 color = g_textures.Sample(g_sam_0, input.uv);
	color.rgb *= 4.0f;

	float lifeRatio = g_data[input.id].curTime / g_data[input.id].lifeTime;

	float alpha = 0.0f;

	if (lifeRatio < 0.5f)
	{
		alpha = smoothstep(0.0f, 0.5f, lifeRatio) * 0.5f;
	}
	else
	{
		alpha = smoothstep(1.0f, 0.6f, lifeRatio) * 0.5f;
	}

	color.a *= alpha;

	return color;
}

struct ComputeShared
{
	int addCount;
	float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
	if (threadIndex.x >= g_int_0)
		return;

	int maxCount = g_int_0;
	int addCount = g_int_1;
	float deltaTime = g_vec2_1.x;
	float accTime = g_vec2_1.y;
	float minLifeTime = g_vec4_0.x;
	float maxLifeTime = g_vec4_0.y;
	float dirX = g_vec4_1.x;
	float dirY = g_vec4_1.y;
	float dirZ = g_vec4_1.z;

	g_shared[0].addCount = addCount;
	GroupMemoryBarrierWithGroupSync();

	if (g_particle[threadIndex.x].alive == 0 && g_shared[0].addCount > 0)
	{
		while (true)
		{
			int remaining = g_shared[0].addCount;
			if (remaining <= 0)
				break;
			int expected = remaining;
			int desired = remaining - 1;
			int originalValue;
			InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);
			if (originalValue == expected)
			{
				g_particle[threadIndex.x].alive = 1;
				break;
			}
		}

		if (g_particle[threadIndex.x].alive == 1)
		{
			g_particle[threadIndex.x].worldDir = normalize(float3(dirX, dirY, dirZ));
			g_particle[threadIndex.x].worldPos = float3(0, 0, 0);
			float x = ((float) threadIndex.x / (float) maxCount) + accTime;
			float3 noise = float3(
				Rand(float2(x, accTime)),
				Rand(float2(x * 0.31f, accTime)),
				Rand(float2(x * 0.59f, accTime)));
			g_particle[threadIndex.x].lifeTime = (maxLifeTime - minLifeTime) * noise.x + minLifeTime;
			g_particle[threadIndex.x].curTime = 0.f;
		}
	}
	else
	{
		g_particle[threadIndex.x].curTime += deltaTime;
		if (g_particle[threadIndex.x].curTime > g_particle[threadIndex.x].lifeTime)
		{
			g_particle[threadIndex.x].alive = 0;
			return;
		}
	}
}
#endif