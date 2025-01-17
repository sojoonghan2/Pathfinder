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

float4 PS_Main(GS_OUT input) : SV_Target
{
    // 파티클 텍스처로 알파값 샘플링
    float4 particleColor = g_textures[0].Sample(g_sam_0, input.uv);
    
    // UV 좌표 왜곡
    float2 distortedUV = input.uv;
    
    // 중심점 기준으로 UV 왜곡
    float2 center = float2(0.5f, 0.5f);
    float2 dir = input.uv - center;
    float dist = length(dir);
    
    // 왜곡 강도 조절 (값이 클수록 더 많이 휘어짐)
    float strength = 0.2f;
    distortedUV += dir * dist * strength;
    
    // 왜곡된 UV로 스크린 텍스처 샘플링
    float4 refractionColor = g_refractionTex.Sample(g_sam_0, distortedUV);
    
    // 파티클 텍스처의 알파값을 이용해 스크린 텍스처 블렌딩
    return float4(refractionColor.rgb, particleColor.a);
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

// 그룹 공유 변수 (모든 스레드에서 공통으로 사용)
groupshared int isGenerated; // 0: 생성되지 않음, 1: 생성됨

// CS_Main
// g_vec2_1 : 델타 시간 / 누적 시간
// g_int_0  : 최대 파티클 수
// g_int_1  : 새로 활성화할 파티클 수
// g_vec4_0 : 최소/최대 수명과 속도
[numthreads(1, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    // 초기화
    if (threadIndex.x == 0)
        isGenerated = 0;
    
    int addCount = g_int_1;
    float maxLifeTime = g_vec4_0.y;
    
    // 활성화 가능한 파티클 수 관리
    g_shared[0].addCount = addCount;
    // 스레드 동기화
    GroupMemoryBarrierWithGroupSync();

    // 비활성 파티클 활성화
    if (g_particle[threadIndex.x].alive == 0 && g_shared[0].addCount > 0 && isGenerated == 0)
    {
        g_particle[threadIndex.x].worldPos = float3(0.0f, 0.0f, 0.0f);
        g_particle[threadIndex.x].lifeTime = g_vec4_0.y;
        g_particle[threadIndex.x].alive = 1;

        // isGenerated 값을 1로 설정
        InterlockedMax(isGenerated, 1);
    }
}

#endif