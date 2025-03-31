#ifndef _LIGHTPILLARPARTICLE_FX_
#define _LIGHTPILLARPARTICLE_FX_

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
    float rotation; // 회전 각도 추가
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
    uint id : SV_InstanceID;
};

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

    // 회전 행렬 생성
    float rotation = g_data[id].rotation;
    float cosTheta = cos(rotation);
    float sinTheta = sin(rotation);
    
    // View Space에서 회전 적용
    float4 positions[4];
    float halfWidth = scale * 1.0f;
    float halfHeight = scale * 4.0f;

    positions[0] = vtx.viewPos + float4(-halfWidth, halfHeight, 0.f, 0.f);
    positions[1] = vtx.viewPos + float4(halfWidth, halfHeight, 0.f, 0.f);
    positions[2] = vtx.viewPos + float4(halfWidth, -halfHeight, 0.f, 0.f);
    positions[3] = vtx.viewPos + float4(-halfWidth, -halfHeight, 0.f, 0.f);

    // 회전 적용
    for (int i = 0; i < 4; i++)
    {
        float x = positions[i].x;
        float y = positions[i].y;
        positions[i].x = x * cosTheta - y * sinTheta;
        positions[i].y = x * sinTheta + y * cosTheta;
    }

    // 최종 위치 설정
    output[0].position = mul(positions[0], g_matProjection);
    output[1].position = mul(positions[1], g_matProjection);
    output[2].position = mul(positions[2], g_matProjection);
    output[3].position = mul(positions[3], g_matProjection);

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

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
    
    float4 color = g_textures.Sample(g_sam_0, uv);

    // 상하 투명도: 위는 진하고 아래는 점점 투명해짐
    float verticalFade = 1.0f - uv.y;

    // 중심 흐릿함: 원형 falloff (0,0)~(1,1) 중앙 기준
    float2 center = float2(0.5f, 0.5f);
    float dist = distance(uv, center);
    float radialFade = saturate(1.0f - dist * 1.5f); // 퍼지는 정도 조절

    // 최종 알파 계산 (더 부드러운 가장자리)
    float fade = verticalFade * radialFade;
    color.a *= fade;

    return color;
}

struct ComputeShared
{
    int addCount;
    int hasSpawned;
    float2 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

[numthreads(3, 1, 1)]
void CS_Main(uint3 threadIndex : SV_DispatchThreadID)
{
    int id = threadIndex.x;
    if (id >= g_int_0)
        return;

    // 새로운 입자 추가
    if (g_particle[id].alive == 0)
    {
        g_particle[id].alive = 1;
            
        // 초기 위치 설정
        g_particle[id].worldPos = float3(0.0f, 1000.0f, 0.0f);
        
        g_particle[id].curTime = 0.0f;
        g_particle[id].lifeTime = g_float_1 + Rand(float2(id, g_float_2)) * 2.0f;
        
    }
    // 기존 입자 업데이트
    if (g_particle[id].alive == 1)
    {
        g_particle[id].curTime += g_vec2_1.x;
        
    }
}
#endif