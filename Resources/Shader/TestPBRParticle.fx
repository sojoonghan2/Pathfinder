#ifndef _TESTPBRPARTICLE_FX_
#define _TESTPBRPARTICLE_FX_

#include "params.fx"
#include "utils.fx"

struct Particle
{
    float3 worldPos;
    float curTime;
    float3 worldDir;
    float lifeTime;
    int alive;
    float3 padding;
};

// StructuredBuffer 정의
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

// Vertex Shader
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

// Geometry Shader
[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output[4] =
    {
        (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f
    };

    VS_OUT vtx = input[0];
    uint id = (uint) vtx.id;
    if (0 == g_data[id].alive)
        return;

    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float scale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;

    // View Space
    output[0].position = vtx.viewPos + float4(-scale, scale, 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(scale, scale, 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(scale, -scale, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-scale, -scale, 0.f, 0.f);

    // Projection Space
    output[0].position = mul(output[0].position, g_matProjection);
    output[1].position = mul(output[1].position, g_matProjection);
    output[2].position = mul(output[2].position, g_matProjection);
    output[3].position = mul(output[3].position, g_matProjection);

    // UV
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    // ID
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

// Pixel Shader
float4 PS_Main(GS_OUT input) : SV_Target
{
    // 파티클 텍스처 샘플링
    float4 particleColor = g_textures.Sample(g_sam_0, input.uv);

    // 원 모양 UV 처리
    float2 center = float2(0.5, 0.5);
    float distance = length(input.uv - center);

    // 굴절 UV 계산
    // 0.5를 빼서 범위를 -0.5 ~ 0.5로 수정(전방향 굴절)
    float2 refractedUV = input.uv + (particleColor.rg - 0.5f) * 0.05f; // * 굴절 강도
    // UV 좌표 범위 제한
    refractedUV = saturate(refractedUV);

    // 렌더 타겟 텍스처에서 굴절된 픽셀 샘플링
    float4 backgroundColor = g_textures2.Sample(g_sam_0, refractedUV);

    // 파티클과 배경 혼합
    return lerp(backgroundColor, particleColor, particleColor.a);
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

[numthreads(1, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.x == 0)
        isGenerated = 0;

    int addCount = g_int_1;
    float maxLifeTime = g_vec4_0.y;

    g_shared[0].addCount = addCount;
    GroupMemoryBarrierWithGroupSync();

    if (g_particle[threadIndex.x].alive == 0 && g_shared[0].addCount > 0 && isGenerated == 0)
    {
        g_particle[threadIndex.x].worldPos = float3(0.0f, 0.0f, 0.0f);
        g_particle[threadIndex.x].lifeTime = g_vec4_0.y;
        g_particle[threadIndex.x].alive = 1;

        InterlockedMax(isGenerated, 1);
    }
}

#endif
