#ifndef _CATACLYSMPARTICLE_FX_
#define _CATACLYSMPARTICLE_FX_

#include "params.fx"
#include "utils.fx"

#define PI 3.141592

struct Particle
{
    float3  worldPos;
    float   curTime;
    float3  worldDir;
    float   lifeTime;
    int     alive;
    float3  padding;
};

// 그래픽스 셰이더
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

// VS_MAIN
// g_float_0        : Start Scale
// g_float_1        : End Scale
// g_textures[0]    : Particle Texture

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

// 파티클의 현재 수명 비율에 따라 크기를 계산하고, 화면에 사각형 형태로 확장
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

    // 기본 크기 설정
    float baseScale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;

    // 랜덤한 크기 변형
    float scaleX = baseScale * (1.2f + (Rand(float2(id, ratio)) * 0.5f - 0.25f));
    float scaleY = baseScale * (1.2f + (Rand(float2(ratio, id)) * 0.5f - 0.25f));

    // 랜덤 회전 추가 (균열이 자연스럽게 보이도록)
    float angle = Rand(float2(id, ratio)) * PI * 2.0f;
    float cosA = cos(angle);
    float sinA = sin(angle);

    // 비대칭한 균열 모양을 위해 다각형 형태 생성
    float2 crackedShape[4] =
    {
        float2(-scaleX * 1.2f, scaleY * 0.8f),
        float2(scaleX * 0.9f, scaleY * 1.2f),
        float2(scaleX * 1.2f, -scaleY * 0.9f),
        float2(-scaleX * 0.8f, -scaleY * 1.1f)
    };

    // 회전 적용
    for (int i = 0; i < 4; i++)
    {
        crackedShape[i] = float2(
            crackedShape[i].x * cosA - crackedShape[i].y * sinA,
            crackedShape[i].x * sinA + crackedShape[i].y * cosA
        );
    }

    // View Space 적용
    output[0].position = vtx.viewPos + float4(crackedShape[0], 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(crackedShape[1], 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(crackedShape[2], 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(crackedShape[3], 0.f, 0.f);

    // Projection 변환
    for (int i = 0; i < 4; i++)
    {
        output[i].position = mul(output[i].position, g_matProjection);
    }

    // uv 및 id 설정
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    for (int i = 0; i < 4; i++)
    {
        output[i].id = id;
    }

    // 삼각형 스트립 추가
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
    
    // 균열 중심 색상
    float3 startColor = float3(0.2f, 0.2f, 0.2f); // 짙은 회색 (바닥)
    float3 endColor = float3(0.8f, 0.8f, 0.8f); // 흰색 (부서지는 경계)
    
    // 부드러운 색상 전환
    float3 color = lerp(startColor, endColor, ratio);

    // 거리 기반 알파값 조정
    float2 center = float2(0.5f, 0.5f);
    float dist = distance(input.uv, center);

    float alpha = saturate(1.0f - dist * 2.0f); // 중심 1.0, 가장자리 0.0
    alpha = pow(alpha, 2.5f); // 더 부드러운 흐려짐 효과

    // 텍스처 샘플링 및 노이즈 적용
    float2 uvOffset = float2(Rand(float2(input.id, ratio)) * 0.1f - 0.05f,
                             Rand(float2(ratio, input.id)) * 0.1f - 0.05f);
    float2 sampledUV = saturate(input.uv + uvOffset);
    
    float4 texColor = g_textures.Sample(g_sam_0, sampledUV);

    return float4(color, alpha) * texColor;
}

struct ComputeShared
{
    int addCount;
    float3 padding;
};

// 컴퓨트 셰이더 입/출력
// 파티클 데이터가 저장된 버퍼
RWStructuredBuffer<Particle> g_particle : register(u0);
// 활성화할 파티클 수를 관리하는 공유 데이터
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

// CS_Main
// g_vec2_1 : 델타 시간 / 누적 시간
// g_int_0  : 최대 파티클 수
// g_int_1  : 새로 활성화할 파티클 수
// g_vec4_0 : 최소/최대 수명과 속도
[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.x >= g_int_0)
        return;

    // 초기 시작 범위
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

    // 활성화 가능한 파티클 수 관리
    g_shared[0].addCount = addCount;
    // 스레드 동기화
    GroupMemoryBarrierWithGroupSync();

    // 비활성 파티클 활성화
    if (g_particle[threadIndex.x].alive == 0 && g_shared[0].addCount > 0)
    {
        // 활성화 가능한 파티클 수 감소
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

        // 대격변 파티클 연산
        if (g_particle[threadIndex.x].alive == 1)
        {
            float x = ((float) threadIndex.x / (float) maxCount) + accTime;
            // 삼각형 꼭짓점 정의
            float2 A = float2(-300.0f, 500.0f);
            float2 B = float2(300.0f, 500.0f);
            float2 C = float2(0.0f, -50.0f);

            // 균일한 삼각형 분포를 위한 수정된 바리센트릭 좌표 생성
            float r1 = Rand(float2(x, accTime));
            float r2 = Rand(float2(x * accTime, accTime));
    
            // 수정된 바리센트릭 좌표 계산
            float u = 1.0f - sqrt(r1);
            float v = r2 * sqrt(r1);
            float w = 1.0f - u - v;
    
            // 삼각형 내부의 랜덤 좌표 계산
            float2 randPosition = u * A + v * B + w * C;
    
            g_particle[threadIndex.x].worldPos = float3(randPosition.x, 0.0f, randPosition.y);
            
            // 위치와 독립적인 수명 설정
            float randomSeed = Rand(float2(accTime, threadIndex.x));
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * randomSeed) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f;
        }
    }
    // 기존 파티클 업데이트
    else
    {
        // 현재 시간 업데이트
        g_particle[threadIndex.x].curTime += deltaTime;

        // 균열이 점점 넓어지도록 속도를 다르게 설정
        float progress = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;

        // 초기 속도는 빠르게, 점점 느려지도록 설정
        float3 velocity = g_particle[threadIndex.x].worldDir; // 파편의 기본 이동 방향
        float initialSpeed = 5000.0f; // 초기 강한 속도 (지면 충격)
        float gravity = -5000.0f; // 중력 (파편이 떨어지게 만듦)
    
        // 랜덤한 바람 효과 적용 (X, Z 방향 확산)
        float windFactor = 0.5f;
        float windX = (Rand(float2(progress, threadIndex.x)) - 0.5f) * windFactor;
        float windZ = (Rand(float2(threadIndex.x, progress)) - 0.5f) * windFactor;

        // 초기에는 빠르게 튀어오름
        if (progress < 0.01f)
        {
            velocity.y += initialSpeed * (1.0f - progress); // 점점 감속
        }
        // 중력 적용 시작
        else if (progress > 0.01f)
        {
            velocity.y += gravity * deltaTime;
        }

        // X, Z 방향 확산 (바람 효과 추가)
        velocity.x += windX * deltaTime;
        velocity.z += windZ * deltaTime;

        // 위치 업데이트
        g_particle[threadIndex.x].worldPos += velocity * deltaTime;

        // 수명이 다한 파티클 제거
        if (g_particle[threadIndex.x].curTime >= g_particle[threadIndex.x].lifeTime)
        {
            g_particle[threadIndex.x].alive = 0;
        }
    }
}

#endif