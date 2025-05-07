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
	float3 velocity;
	float rotation;
	float randomSeed;
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
	float rotation : ROTATION;
	float randomSeed : RANDOM_SEED;
};

VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT) 0.f;

	float3 worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;
	worldPos += g_data[input.id].worldPos;

	output.viewPos = mul(float4(worldPos, 1.f), g_matView);
	output.uv = input.uv;
	output.id = input.id;
	output.rotation = g_data[input.id].rotation;
	output.randomSeed = g_data[input.id].randomSeed;

	return output;
}

struct GS_OUT
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD;
	uint id : SV_InstanceID;
	float lifeRatio : LIFE_RATIO;
	float randomSeed : RANDOM_SEED;
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
    
	float widthScale = scaleBase * (1.0f + 0.3f * sin(vtx.randomSeed * 6.28f));
	float heightScale = scaleBase * (1.0f + 0.2f * cos(vtx.randomSeed * 4.13f)) * 1.8f;
    
	float4 centerPos = vtx.viewPos;
    
	float2 corners[4] =
	{
		float2(-widthScale, heightScale),
        float2(widthScale, heightScale),
        float2(widthScale, -heightScale),
        float2(-widthScale, -heightScale)
	};
    
	float rotationAngle = vtx.rotation + lifeRatio * PI * 0.5f;
	float2x2 rotMat = Create2DRotationMatrix(rotationAngle);
    
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
		output[i].randomSeed = vtx.randomSeed;
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
	float randomSeed = input.randomSeed;
    
	float dist = distance(input.uv, float2(0.5f, 0.5f)) * 2.0f;
    
	float3 coreColor = float3(1.0f, 0.2f, 0.1f);
	float3 midColor = float3(1.0f, 0.9f, 0.3f);
	float3 edgeColor = float3(1.0f, 1.0f, 1.0f);
    
	float3 baseColor;
	if (dist < 0.7f)
	{
		baseColor = lerp(coreColor, midColor, smoothstep(0.0f, 0.7f, dist));
	}
	else if (dist < 0.9f)
	{
		baseColor = lerp(midColor, edgeColor, smoothstep(0.7f, 0.9f, dist));
	}
	else
	{
		baseColor = edgeColor;
	}
    
	baseColor = lerp(baseColor, edgeColor, lifeRatio * 0.4f);
    
	float brightness = 5.0f + 3.0f * sin(randomSeed * 7.92f);
	float colorShift = 0.1f * sin(randomSeed * 3.14f);
    
	color.rgb *= baseColor * brightness;
	color.rgb += float3(colorShift, colorShift * 0.5f, 0.0f);
    
	float flicker1 = sin(lifeRatio * PI * 15.0f + randomSeed * 6.28f);
	float flicker2 = sin(lifeRatio * PI * 8.0f + randomSeed * 3.14f);
	float flickerMix = saturate(0.7f * flicker1 + 0.3f * flicker2);
    
	float initialPulse = saturate(1.2f - lifeRatio * 8.0f);
    
	float fadeOut = saturate(1.0f - pow(lifeRatio, 0.5f) * 1.2f);
    
	float alpha = saturate(fadeOut * (0.8f + 0.2f * flickerMix + initialPulse * 0.5f));
    
	float edgeDist = distance(input.uv, float2(0.5f, 0.5f)) * 2.0f;
	float edgeGlow = saturate(1.0f - edgeDist * edgeDist * 1.2f);
	edgeGlow = pow(edgeGlow, 1.5f + lifeRatio * 2.5f);
    
	color.a *= alpha * edgeGlow;
    
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
			float x = ((float) threadIndex.x / (float) maxCount) + accTime;
			float3 noise = float3(
                Rand(float2(x, accTime)),
                Rand(float2(x * 0.31f, accTime)),
                Rand(float2(x * 0.59f, accTime)));
                
			float3 baseDir = normalize(float3(dirX, dirY, dirZ));
			float3 randDir = normalize(noise * 2.0f - 1.0f) * 0.2f;
			g_particle[threadIndex.x].worldDir = normalize(baseDir + randDir);
            
			g_particle[threadIndex.x].worldPos = float3(noise.x * 0.5f - 0.25f,
                                                     noise.y * 0.5f - 0.25f,
                                                     noise.z * 0.5f - 0.25f);
            
			g_particle[threadIndex.x].velocity = g_particle[threadIndex.x].worldDir * (10.0f + noise.x * 5.0f) +
                                             float3(noise.y * 2.0f - 1.0f,
                                                   noise.z * 2.0f - 1.0f,
                                                   noise.x * 2.0f - 1.0f) * 3.0f;
            
			g_particle[threadIndex.x].rotation = noise.y * PI * 2.0f;
            
			g_particle[threadIndex.x].randomSeed = noise.x;
            
			g_particle[threadIndex.x].lifeTime = (maxLifeTime - minLifeTime) * noise.z + minLifeTime;
			g_particle[threadIndex.x].curTime = 0.f;
		}
	}
	else if (g_particle[threadIndex.x].alive == 1)
	{
		g_particle[threadIndex.x].curTime += deltaTime;
		if (g_particle[threadIndex.x].curTime > g_particle[threadIndex.x].lifeTime)
		{
			g_particle[threadIndex.x].alive = 0;
			return;
		}
        
		float lifeRatio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
        
		float velocityFactor = 1.0f - lifeRatio * 0.8f;
        
		g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].velocity * deltaTime * velocityFactor;
        
		g_particle[threadIndex.x].rotation += deltaTime * (1.0f - lifeRatio) * 2.0f;
	}
}
#endif