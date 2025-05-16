#ifndef _PORTALFRAMEPARTICLE_FX_
#define _PORTALFRAMEPARTICLE_FX_

#include "ShaderParams.fx"
#include "utils.fx"

#define PI 3.141592

struct Particle
{
    float3 worldPos;
    float curTime;
    float3 worldDir;
    float lifeTime;
    int alive;
    float rotationAngle;
    float2 padding;
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
    float4 color : COLOR;
    uint id : SV_InstanceID;
};

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };

    VS_OUT vtx = input[0];
    uint id = (uint) vtx.id;
    if (g_data[id].alive == 0)
        return;

    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float scale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;

    float rotAngle = g_data[id].rotationAngle;
    float sinA = sin(rotAngle);
    float cosA = cos(rotAngle);

    float4 corners[4] =
    {
        float4(-scale, scale, 0.f, 0.f),
        float4(scale, scale, 0.f, 0.f),
        float4(scale, -scale, 0.f, 0.f),
        float4(-scale, -scale, 0.f, 0.f)
    };

    for (int i = 0; i < 4; i++)
    {
        float x = corners[i].x;
        float y = corners[i].y;
        corners[i].x = x * cosA - y * sinA;
        corners[i].y = x * sinA + y * cosA;
    }

    output[0].position = mul(vtx.viewPos + corners[0], g_matProjection);
    output[1].position = mul(vtx.viewPos + corners[1], g_matProjection);
    output[2].position = mul(vtx.viewPos + corners[2], g_matProjection);
    output[3].position = mul(vtx.viewPos + corners[3], g_matProjection);

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    float4 glowColor = float4(1.0f, 0.9f, 0.3f, ratio * (1 - ratio) * 4.0f);
    output[0].color = glowColor;
    output[1].color = glowColor;
    output[2].color = glowColor;
    output[3].color = glowColor;

    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;

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
    float2 uv = input.uv;
    float2 offset = uv - float2(0.5f, 0.5f);
    float dist = length(offset); // 0: Áß½É, 1: ¿Ü°û

    float ratio = g_data[input.id].curTime / g_data[input.id].lifeTime;
    
    float alpha = exp(-pow(dist * 3.5f, 2.0f));
    alpha *= pow(ratio, 0.1f);
    alpha *= exp(-dist * 4.5f * 2);
    
    float flicker = 0.95f + 0.05f * sin(g_data[input.id].rotationAngle * 30.0f + ratio * 30.0f);
    
    float3 potalColor = float3(0.8f, 0.2f, 0.1f);
    
    float4 color = float4(potalColor, 1.0f) * flicker * alpha;

    if (color.a < 0.01f)
        discard;

    return color;
}

// Compute Shader
struct ComputeShared
{
    int addCount;
    float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

groupshared int isGenerated;

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

    g_shared[0].addCount = addCount;
    GroupMemoryBarrierWithGroupSync();

    if (g_particle[threadIndex.x].alive == 0)
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
            float r1 = Rand(float2(x, accTime));
            float r2 = Rand(float2(x * accTime, accTime));

            float angle = 2.0f * PI * r1;
            float radius = 500.0f;

            g_particle[threadIndex.x].worldPos = float3(radius * cos(angle), radius * sin(angle), 0.0f);
            g_particle[threadIndex.x].worldDir = normalize(float3(cos(angle), sin(angle), 0.0f));
            g_particle[threadIndex.x].lifeTime = lerp(minLifeTime, maxLifeTime, r2);
            g_particle[threadIndex.x].curTime = 0.f;
            g_particle[threadIndex.x].rotationAngle = angle;
        }
    }
    else
    {
        g_particle[threadIndex.x].curTime += deltaTime;

        if (g_particle[threadIndex.x].curTime >= g_particle[threadIndex.x].lifeTime)
        {
            g_particle[threadIndex.x].alive = 0;
        }
        else
        {
            float angle = g_particle[threadIndex.x].rotationAngle;
            float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;

            float currentRadius = (ratio < 0.3f)
                ? lerp(1000.0f, 500.0f, ratio / 0.3f)
                : 500.0f;

            g_particle[threadIndex.x].rotationAngle -= deltaTime * 4.0f;

            float theta = g_particle[threadIndex.x].rotationAngle;
            g_particle[threadIndex.x].worldPos = float3(currentRadius * cos(theta), currentRadius * sin(theta), 0.0f);
        }
    }
}

#endif
