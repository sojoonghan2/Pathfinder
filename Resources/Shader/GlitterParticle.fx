#ifndef _GLITTERPARTICLE_FX_
#define _GLITTERPARTICLE_FX_

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
    positions[0] = vtx.viewPos + float4(-scale, scale, 0.f, 0.f);
    positions[1] = vtx.viewPos + float4(scale, scale, 0.f, 0.f);
    positions[2] = vtx.viewPos + float4(scale, -scale, 0.f, 0.f);
    positions[3] = vtx.viewPos + float4(-scale, -scale, 0.f, 0.f);

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
    float ratio = g_data[input.id].curTime / g_data[input.id].lifeTime;
    
    // 진행도에 따른 색상 변화
    float3 baseColor = float3(0.6f, 0.6f, 0.6f); // 기본 회색
    float3 startColor = float3(0.8f, 0.7f, 0.5f); // 황토색
    float3 endColor = float3(0.3f, 0.3f, 0.3f); // 어두운 회색
    
    float3 color;
    if (ratio < 0.3f)
    {
        // 초기: 밝은 색에서 기본색으로
        color = lerp(startColor, baseColor, ratio / 0.3f);
    }
    else
    {
        // 후기: 기본색에서 어두운 색으로
        color = lerp(baseColor, endColor, (ratio - 0.3f) / 0.7f);
    }
    
    // 균열 패턴 생성
    float2 center = float2(0.5f, 0.5f);
    float dist = distance(input.uv, center);
    
    // 불규칙한 균열 패턴
    float noise = Rand(float2(input.id, ratio)) * 0.2f;
    float crackPattern = saturate(1.0f - dist * (2.0f + noise));
    crackPattern = pow(crackPattern, 1.5f);
    
    float4 texColor = g_textures.Sample(g_sam_0, input.uv);
    
    // 알파값 수정
    float fadeFactor = smoothstep(0.5f, 1.0f, ratio); // 50%부터 점진적 페이드아웃
    float alpha = crackPattern * (1.0f - pow(fadeFactor, 2.0f)); // 천천히 감소
    
    // 최종 색상
    return float4(color * texColor.rgb, alpha * texColor.a);
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

    float minX = -100.0f, maxX = 100.0f;
    float minZ = -100.0f, maxZ = 100.0f;
    int maxCount = g_int_0;
    int addCount = g_int_1;
    float deltaTime = g_vec2_1.x;
    float accTime = g_vec2_1.y;
    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;
    float minSpeed = g_vec4_0.z;
    float maxSpeed = g_vec4_0.w;

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
            // 사각형 패턴으로 초기 위치 설정
            float size = 100.0f; // 사각형의 한 변 길이
            g_particle[threadIndex.x].worldPos = float3(
                0.0f, 0.0f, 0.0f
            );

            // 상승 방향 설정
            g_particle[threadIndex.x].worldDir = float3(0.0f, 1.0f, 0.0f);
            g_particle[threadIndex.x].lifeTime = minLifeTime;
            g_particle[threadIndex.x].curTime = 0.0f;
        }
    }
}

#endif