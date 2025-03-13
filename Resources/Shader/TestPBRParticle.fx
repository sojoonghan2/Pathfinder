<<<<<<< Updated upstream:Resources/Shader/TestPBRParticle.fx
#ifndef _TESTPBRPARTICLE_FX_
#define _TESTPBRPARTICLE_FX_
=======
#ifndef _REFRECTION_PARTICLE_FX_
#define _REFRECTION_PARTICLE_FX_
>>>>>>> Stashed changes:Resources/Shader/refraction_particle.fx

#include "params.fx"
#include "utils.fx"

struct RefractionParticle
{
<<<<<<< Updated upstream:Resources/Shader/TestPBRParticle.fx
    float3 worldPos;
    float curTime;
    float3 worldDir;
    float lifeTime;
    int alive;
    float3 padding;
=======
    float3 worldPos; // 12
    float curTime; // 4
    float3 worldDir; // 12
    float lifeTime; // 4
    int alive; // 4
    float3 padding; // 12
>>>>>>> Stashed changes:Resources/Shader/refraction_particle.fx
};

// StructuredBuffer 정의
StructuredBuffer<RefractionParticle> g_data : register(t9);

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

<<<<<<< Updated upstream:Resources/Shader/TestPBRParticle.fx
// Vertex Shader
=======
// VS_MAIN
// g_float_0        : Start Scale
// g_float_1        : End Scale
// g_textures[0]    : Particle Texture

>>>>>>> Stashed changes:Resources/Shader/refraction_particle.fx
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
    
    // 알파 값이 0.01f보다 작으면(컬러가 없으면) 원래 파티클 컬러 유지
    if (particleColor.a < 0.01f)
    {
        return particleColor;
    }

    // 굴절 UV 계산
    // 0.0 ~ 1.0이라는 범위를 사용하기 위해 rg 채널을 사용
    // 0.5를 빼서 범위를 -0.5 ~ 0.5로 수정(전방향 굴절)
    float2 refractedUV = input.uv + (particleColor.rg - 0.5f) * 0.05f; // * 굴절 강도
    // UV 좌표 범위 제한
    refractedUV = saturate(refractedUV);

    // 렌더 타겟의 컬러 텍스처에서 굴절된 픽셀 샘플링
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

<<<<<<< Updated upstream:Resources/Shader/TestPBRParticle.fx
RWStructuredBuffer<Particle> g_particle : register(u0);
=======
// 컴퓨트 셰이더 입/출력
// 파티클 데이터가 저장된 버퍼
RWStructuredBuffer<RefractionParticle> g_particle : register(u0);
// 활성화할 파티클 수를 관리하는 공유 데이터
>>>>>>> Stashed changes:Resources/Shader/refraction_particle.fx
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

groupshared int isGenerated;

[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.x >= g_int_0)
        return;

    int maxCount = g_int_0;
    int addCount = g_int_1;
    int frameNumber = g_int_2;
    float deltaTime = g_vec2_1.x;
    float accTime = g_vec2_1.y;
    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;
    float minSpeed = g_vec4_0.z;
    float maxSpeed = g_vec4_0.w;

    g_shared[0].addCount = addCount;
    GroupMemoryBarrierWithGroupSync();

<<<<<<< Updated upstream:Resources/Shader/TestPBRParticle.fx
    if (g_particle[threadIndex.x].alive == 0)
=======
    // 1. 비활성 파티클 활성화
    if (g_particle[threadIndex.x].alive == 0 && g_shared[0].addCount > 0)
>>>>>>> Stashed changes:Resources/Shader/refraction_particle.fx
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
            float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));

            // [0.5~1] -> [0~1]
            float3 noise =
            {
                2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1
            };

            // [0~1] -> [-1~1]
            float3 dir = (noise - 0.5f) * 2.f;

            g_particle[threadIndex.x].worldDir = normalize(dir);
            g_particle[threadIndex.x].worldPos = (noise.xyz - 0.5f) * 25;
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * noise.x) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f;
        }
    }
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
        g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
    }
}

#endif