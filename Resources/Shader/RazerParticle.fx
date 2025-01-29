#ifndef _RAZERPARTICLE_FX_
#define _RAZERPARTICLE_FX_

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
    float lifeRatio : TEXCOORD1;
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

    // 레이저 특성을 반영한 길이와 두께 조절
    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float thickness = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f; // 두께
    float length = ((g_float_1 - g_float_0) * ratio + g_float_0) * 2.f; // 길이

    // View Space에서 수평 방향으로 레이저 확장
    // 수직(y축) 대신 수평(x축)으로 확장하도록 변경
    output[0].position = vtx.viewPos + float4(-length, thickness, 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(length, thickness, 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(length, -thickness, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-length, -thickness, 0.f, 0.f);

    // Projection Space 변환
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        output[i].position = mul(output[i].position, g_matProjection);
        output[i].lifeRatio = ratio;
        output[i].id = id;
    }

    // UV 좌표 (가로로 늘어난 형태)
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    // 삼각형 스트립 생성
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
    // 레이저 효과를 위한 색상 계산
    float4 texColor = g_textures.Sample(g_sam_0, input.uv);
    
    // 시간에 따라 변화하는 레이저 색상
    float time = g_data[input.id].curTime * 5.0f;
    float3 baseColor = float3(0.0f, 0.7f, 1.0f); // 기본 레이저 색상 (청색)
    float3 glowColor = float3(1.0f, 1.0f, 1.0f); // 백색 글로우
    
    // 펄스 효과
    float pulse = (sin(time) * 0.5f + 0.5f);
    
    // 중심선 효과
    float centerLine = 1.0f - abs(input.uv.x - 0.5f) * 2.0f;
    centerLine = pow(centerLine, 3.0f); // 더 선명한 중심선
    
    // 최종 색상 계산
    float3 finalColor = lerp(baseColor, glowColor, centerLine * pulse);
    
    // 알파값 계산
    float alpha = texColor.a * centerLine;
    
    // 끝부분 페이드아웃
    float edgeFade = 1.0f - abs(input.uv.y - 0.5f) * 2.0f;
    alpha *= edgeFade;
    
    // 수명 기반 페이드아웃
    float fadeOut = 1.0f - input.lifeRatio;
    alpha *= fadeOut;
    
    return float4(finalColor * 1.5f, alpha); // 전체적인 밝기 증가
}

// 컴퓨트 셰이더 부분
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

    // 레이저 시작 위치 범위 조정 (더 집중된 형태로)
    float minX = -10.0f, maxX = 10.0f;
    float minY = -10.0f, maxY = 10.0f;
    float minZ = -10.0f, maxZ = 10.0f;

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
            float x = ((float) threadIndex.x / (float) maxCount) + accTime;
            float y = ((float) threadIndex.y / (float) maxCount) + accTime;

            // 레이저 시작점 설정
            g_particle[threadIndex.x].worldPos = float3(
                lerp(minX, maxX, Rand(float2(x, accTime))),
                lerp(minY, maxY, Rand(float2(y, accTime))),
                minZ
            );

            // 직선 방향 유지
            g_particle[threadIndex.x].worldDir = normalize(float3(0.0f, 0.0f, 1.0f));
            
            // 수명 및 시간 설정
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * Rand(float2(x * accTime, accTime))) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f;
        }
    }
    else if (g_particle[threadIndex.x].alive == 1)
    {
        g_particle[threadIndex.x].curTime += deltaTime;
        if (g_particle[threadIndex.x].lifeTime < g_particle[threadIndex.x].curTime)
        {
            g_particle[threadIndex.x].alive = 0;
            return;
        }

        float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
        float speed = (maxSpeed - minSpeed) * ratio + minSpeed;
        
        // 직선 운동 유지
        g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
    }
}

#endif