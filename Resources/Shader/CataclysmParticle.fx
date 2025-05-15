#ifndef _CATACLYSMPARTICLE_FX_
#define _CATACLYSMPARTICLE_FX_

#include "ShaderParams.fx"
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

    int maxCount = g_int_0;
    int addCount = g_int_1;
    float deltaTime = g_vec2_1.x;
    float accTime = g_vec2_1.y;
    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;

    g_shared[0].addCount = addCount;
    GroupMemoryBarrierWithGroupSync();

    // 비활성 파티클 활성화
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
            
            // 부채꼴 모양으로 파티클 생성 (Z+ 방향으로 변경)
            float radius = lerp(50.0f, 200.0f, Rand(float2(x, accTime)));
            float angle = lerp(-PI * 0.3f, PI * 0.3f, Rand(float2(x * accTime, accTime)));
            
            float posX = radius * sin(angle);
            float posZ = radius * cos(angle);
            
            g_particle[threadIndex.x].worldPos = float3(posX, 0.0f, posZ);
            
            // 초기 방향 설정 (Z+ 방향으로 바깥쪽으로)
            float3 direction = normalize(float3(posX,
                lerp(100.0f, 300.0f, Rand(float2(x, angle))), // 높이
                posZ));
            
            // 초기 속도 설정
            float initialVelocity = lerp(1000.0f, 2000.0f, Rand(float2(accTime, x)));
            g_particle[threadIndex.x].worldDir = direction * initialVelocity;
            
            // 수명 설정
            g_particle[threadIndex.x].lifeTime = lerp(minLifeTime, maxLifeTime,
                Rand(float2(accTime, threadIndex.x)));
            g_particle[threadIndex.x].curTime = 0.f;
        }
    }
    // 활성 파티클 업데이트
    else if (g_particle[threadIndex.x].alive == 1)
    {
        g_particle[threadIndex.x].curTime += deltaTime;
        float progress = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;

        float3 velocity = g_particle[threadIndex.x].worldDir;
        
        // 중력 및 감속 효과
        float gravity = -981.0f;
        float dragFactor = 1.0f;
        
        // 진행도에 따른 중력 영향 조절
        if (progress < 0.2f)
        {
            // 초기에는 중력 영향 적게
            velocity.y += gravity * deltaTime * 0.5f;
            dragFactor = 0.3f;
        }
        else
        {
            // 이후 중력 영향 증가
            velocity.y += gravity * deltaTime;
            dragFactor = lerp(0.3f, 1.0f, (progress - 0.2f) / 0.8f);
        }
        
        // 공기 저항
        velocity *= (1.0f - dragFactor * deltaTime);
        
        // 지면 충돌 처리
        if (g_particle[threadIndex.x].worldPos.y < 0.0f)
        {
            g_particle[threadIndex.x].worldPos.y = 0.0f;
            velocity.y = -velocity.y * 0.4f; // 탄성 계수
            velocity.xz *= 0.9f; // 마찰
        }
        
        g_particle[threadIndex.x].worldDir = velocity;
        g_particle[threadIndex.x].worldPos += velocity * deltaTime;

        if (g_particle[threadIndex.x].curTime >= g_particle[threadIndex.x].lifeTime * 0.95f)
        {
            float fadeProgress = (g_particle[threadIndex.x].curTime - g_particle[threadIndex.x].lifeTime * 0.95f)
                           / (g_particle[threadIndex.x].lifeTime * 0.05f);
            if (fadeProgress >= 1.0f)
            {
                g_particle[threadIndex.x].alive = 0;
            }
        }
    }
}

#endif