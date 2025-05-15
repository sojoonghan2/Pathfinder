#ifndef _MUZZLEFLAME_PARTICLE_FX_
#define _MUZZLEFLAME_PARTICLE_FX_

#include "ShaderParams.fx"
#include "utils.fx"

#define PI 3.141592

// 기존 구조체 유지
struct Particle
{
	float3 worldPos;
	float curTime;
	float3 worldDir;
	float lifeTime;
	int alive;
	float3 padding; // padding 유지
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
	float lifeRatio : LIFE_RATIO;
};

float2x2 Create2DRotationMatrix(float angle)
{
	float s = sin(angle);
	float c = cos(angle);
	return float2x2(c, -s, s, c);
}

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
	GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };

	VS_OUT vtx = input[0];
	uint id = (uint) vtx.id;
	if (0 == g_data[id].alive)
		return;

	float lifeRatio = g_data[id].curTime / g_data[id].lifeTime;
	
	float scaleBase = ((g_float_1 - g_float_0) * lifeRatio + g_float_0);
	
	float pseudoRandom = frac(sin(dot(float2(id, g_data[id].curTime), float2(12.9898f, 78.233f))) * 43758.5453f);
	float pseudoRandom2 = frac(sin(dot(float2(id * 1.234f, g_data[id].curTime * 0.876f), float2(27.1653f, 57.713f))) * 31268.5123f);
	
	float widthScale = scaleBase * (1.0f + 0.2f * sin(pseudoRandom * 6.28f));
	float heightScale = scaleBase * (1.0f + 0.15f * cos(pseudoRandom2 * 4.13f)) * 1.5f;
	
	float4 centerPos = vtx.viewPos;
	
	float rotationAngle = pseudoRandom * PI * 2.0f + lifeRatio * PI * 0.5f;
	float2x2 rotMat = Create2DRotationMatrix(rotationAngle);
	
	float2 corners[4] =
	{
		float2(-widthScale, heightScale),
		float2(widthScale, heightScale),
		float2(widthScale, -heightScale),
		float2(-widthScale, -heightScale)
	};
	
	for (int i = 0; i < 4; ++i)
	{
		corners[i] = mul(rotMat, corners[i]);
		
		output[i].position = centerPos + float4(corners[i], 0.f, 0.f);
		output[i].position = mul(output[i].position, g_matProjection);
	}
	
	output[0].uv = float2(0.f, 0.f);
	output[1].uv = float2(1.f, 0.f);
	output[2].uv = float2(1.f, 1.f);
	output[3].uv = float2(0.f, 1.f);
	
	for (int i = 0; i < 4; ++i)
	{
		output[i].id = id;
		output[i].lifeRatio = lifeRatio;
	}
	
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
	
	float lifeRatio = input.lifeRatio;
	uint id = input.id;
	
	float pseudoRandom = frac(sin(dot(float2(id, lifeRatio * 100.0f), float2(12.9898f, 78.233f))) * 43758.5453f);
	
	float3 coreColor = lerp(float3(1.0f, 0.9f, 0.5f), float3(1.0f, 0.6f, 0.2f), lifeRatio);
	
	float brightness = 5.0f + 2.0f * sin(pseudoRandom * 7.92f);
	
	color.rgb *= coreColor * brightness;
	
	float flicker1 = sin(lifeRatio * PI * 15.0f + pseudoRandom * 6.28f);
	float flicker2 = sin(lifeRatio * PI * 8.0f + pseudoRandom * 3.14f);
	float flickerMix = saturate(0.7f * flicker1 + 0.3f * flicker2);
	
	float initialPulse = saturate(1.2f - lifeRatio * 8.0f);
	
	float fadeOut = saturate(1.0f - pow(lifeRatio, 0.5f) * 1.2f);
	
	float alpha = fadeOut * (0.8f + 0.2f * flickerMix + initialPulse * 0.5f);
	
	float edgeDist = distance(input.uv, float2(0.5f, 0.5f)) * 2.0f;
	float edgeGlow = saturate(1.0f - edgeDist * edgeDist);
	edgeGlow = pow(edgeGlow, 1.0f + lifeRatio * 2.0f);
	
	color.a *= alpha * edgeGlow * 0.7f;
	
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
			// 파티클 초기화
			float x = ((float) threadIndex.x / (float) maxCount) + accTime;
			float3 noise = float3(
				Rand(float2(x, accTime)),
				Rand(float2(x * 0.31f, accTime)),
				Rand(float2(x * 0.59f, accTime)));
				
			// 총구 방향에 약간의 무작위성 추가
			float3 baseDir = normalize(float3(dirX, dirY, dirZ));
			float3 randDir = normalize(noise * 2.0f - 1.0f) * 0.15f;
			g_particle[threadIndex.x].worldDir = normalize(baseDir + randDir);
			
			// 총구 주변에 파티클 분산 배치
			g_particle[threadIndex.x].worldPos = float3(
				noise.x * 0.2f - 0.1f,
				noise.y * 0.2f - 0.1f,
				noise.z * 0.2f - 0.1f
			);
			
			// 수명 설정
			g_particle[threadIndex.x].lifeTime = (maxLifeTime - minLifeTime) * noise.z + minLifeTime;
			g_particle[threadIndex.x].curTime = 0.f;
		}
	}
	else if (g_particle[threadIndex.x].alive == 1)
	{
		// 파티클 업데이트
		g_particle[threadIndex.x].curTime += deltaTime;
		if (g_particle[threadIndex.x].curTime > g_particle[threadIndex.x].lifeTime)
		{
			g_particle[threadIndex.x].alive = 0;
			return;
		}
		
		float lifeRatio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
		
		// 시간에 따른 속도 감소
		float velocityFactor = 1.0f - lifeRatio * 0.7f;
		
		// 파티클 움직임
		g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * deltaTime * velocityFactor * 2.0f;
	}
}

#endif