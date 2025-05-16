#include "ShaderParams.hlsl"
#include "utils.hlsl"

struct Particle
{
    float3  worldPos;   // 12
    float   curTime;    // 4
    float3  worldDir;   // 12
    float   lifeTime;   // 4
    int     alive;      // 4
    float3 padding;     // 12
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
        (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f
    };

    VS_OUT vtx = input[0];
    uint id = (uint)vtx.id;
    if (0 == g_data[id].alive)
        return;

    // 파티클 수명에 따른 진행률 계산
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

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

// PS_Main 수정 - 생명주기에 따른 색상 변화 추가
float4 PS_Main(GS_OUT input) : SV_Target
{
    uint id = input.id;
    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    
    float4 color = g_textures.Sample(g_sam_0, input.uv);
    
    // 생명 주기에 따른 색상 전환
    if (ratio < 0.4f)
    {
        // 초기 단계: 밝은 불 색상 (노랑/주황)
        color *= float4(1.0f, 0.7f + 0.3f * (1 - ratio / 0.4f), 0.2f, 1.0f);
    }
    else if (ratio < 0.7f)
    {
        // 중간 단계: 불에서 연기로 전환
        float transitionRatio = (ratio - 0.4f) / 0.3f;
        float3 fireColor = float3(1.0f, 0.7f, 0.2f);
        float3 smokeColor = float3(0.3f, 0.3f, 0.3f);
        color.rgb *= lerp(fireColor, smokeColor, transitionRatio);
    }
    else
    {
        // 마지막 단계: 연기 색상으로 점점 투명해짐
        float fadeRatio = (ratio - 0.7f) / 0.3f;
        color *= float4(0.3f, 0.3f, 0.3f, 0.8f - fadeRatio);
    }
    
    return color;
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
    int frameNumber = g_int_2;
    int particleType = g_int_3;
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
            
            // 원자적으로(더 이상 쪼갤 수 없는 정도)로 addCount 값 감소
            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);
            
            // 성공적으로 값을 감소시킨 스레드는 해당 파티클을 활성화(alive = 1)
            if (originalValue == expected)
            {
                g_particle[threadIndex.x].alive = 1;
                break;
            }
        }

        // 얼음 파티클 연산
        if (g_particle[threadIndex.x].alive == 1)
        {
            float x = ((float) threadIndex.x / (float) maxCount) + accTime;

            float r1 = Rand(float2(x, accTime));
            float r2 = Rand(float2(x * accTime, accTime));
            float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));

            // 난수 생성
            float3 noise =
            {
                2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1
            };

            // 불은 주로 위쪽으로 올라가므로 Y방향에 가중치를 둠
            float3 dir = normalize(float3(noise.x * 0.5f, 1.0f + noise.y * 0.3f, noise.z * 0.5f));
    
            // 파티클 초기 위치는 작은 반경 내에서 랜덤하게 생성
            g_particle[threadIndex.x].worldDir = dir;
            g_particle[threadIndex.x].worldPos = float3((noise.x - 0.5f) * 10.0f, (noise.y - 0.5f) * 2.0f, (noise.z - 0.5f) * 10.0f);
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * r1) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f;
        }
        
    }
    // 기존 파티클 업데이트
    else
    {
        g_particle[threadIndex.x].curTime += deltaTime;
        if (g_particle[threadIndex.x].lifeTime < g_particle[threadIndex.x].curTime)
        {
            g_particle[threadIndex.x].alive = 0;
            return;
        }

        float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
        float speed = (maxSpeed - minSpeed) * ratio + minSpeed;
    
        // 파티클 상승 속도는 생명주기에 따라 변함
        // 초기에는 빠르게 올라가다가 나중에는 더 느리게 퍼지는 효과
        float3 currentDir = g_particle[threadIndex.x].worldDir;
        if (ratio > 0.4f)
        {
            // 연기 단계에서는 더 랜덤한 움직임과 느린 상승
            float turbulence = sin(ratio * 10.0f + g_particle[threadIndex.x].worldPos.x) * 0.2f;
            currentDir = normalize(float3(
            currentDir.x + turbulence,
            currentDir.y * (1.0f - ratio * 0.7f), // Y 방향 속도 감소
            currentDir.z + turbulence
        ));
        }
    
        g_particle[threadIndex.x].worldPos += currentDir * speed * deltaTime;
    }
}