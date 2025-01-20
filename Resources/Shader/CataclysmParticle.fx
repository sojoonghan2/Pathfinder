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

    // 파티클 수명 진행률 비율 계산
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

    // uv
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    // id
    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;

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

    // 색상: 빨강 → 어두운 빨강 → 검은색
    float3 startColor = float3(1.0f, 1.0f, 1.0f); // 밝은 빨강
    float3 endColor = float3(0.0f, 0.0f, 0.0f); // 검은색

    float3 color = lerp(startColor, endColor, ratio);

    // 투명도 효과
    float alpha = 0.8f - ratio; // 서서히 투명해짐

    float4 texColor = g_textures.Sample(g_sam_0, input.uv);
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
        
        // 진행률 계산 (0~1 사이 값)
        float progress = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
        
        // Z축 속도 계산
        float ySpeed;
        if (progress < 0.03f)
        {
            // 처음에는 빠르게 -z 방향으로 이동
            ySpeed = 300.0f; // 빠른 속도, 값은 조정 가능
        }
        else if (progress > 0.2f)
        {
            // 0.5 이후에는 천천히 +z 방향으로 이동
            ySpeed = -50.0f; // 느린 속도, 값은 조정 가능
        }
        
        // 위치 업데이트
        g_particle[threadIndex.x].worldPos.y += ySpeed * deltaTime;
        
        // 수명이 다한 파티클 제거
        if (g_particle[threadIndex.x].curTime >= g_particle[threadIndex.x].lifeTime)
        {
            g_particle[threadIndex.x].alive = 0;
        }
    }
}

#endif