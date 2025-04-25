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
    float rotationAngle;
    float2 padding;
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
    float4 worldPos : POSITION; // 변경: viewPos에서 worldPos로 변경
    float2 uv : TEXCOORD;
    float id : ID;
    float3 worldDir : DIRECTION; // 추가: 방향 정보 전달
    float rotationAngle : ROTATION; // 추가: 회전 각도 전달
};

// Vertex Shader
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    float3 worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;
    worldPos += g_data[input.id].worldPos;

    output.worldPos = float4(worldPos, 1.f); // 변경: 월드 위치 저장
    output.uv = input.uv;
    output.id = input.id;
    output.worldDir = g_data[input.id].worldDir; // 방향 정보 전달
    output.rotationAngle = g_data[input.id].rotationAngle; // 회전 각도 전달

    return output;
}

struct GS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
    uint id : SV_InstanceID;
};

// Geometry Shader - 카메라 회전에 영향받지 않도록 수정
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
    
    // Get rotation angle
    float rotAngle = vtx.rotationAngle;
    
    // Calculate sine and cosine for rotation
    float sinAngle = sin(rotAngle);
    float cosAngle = cos(rotAngle);
    
    // 파티클이 +Z 방향을 바라보도록 고정 좌표계 설정
    float3 front = float3(0.0f, 0.0f, 1.0f); // +Z 방향 (바라보는 방향)
    float3 right = float3(1.0f, 0.0f, 0.0f); // +X 방향 (오른쪽)
    float3 up = float3(0.0f, 1.0f, 0.0f); // +Y 방향 (위쪽)
    
    // 월드 공간에서 쿼드 코너 계산 (XY 평면에 존재)
    float4 worldCorners[4];
    
    // 회전된 평면의 코너 계산 (XY 평면에서 회전)
    worldCorners[0] = vtx.worldPos + float4((-scale * cosAngle - scale * sinAngle) * right + (-scale * sinAngle + scale * cosAngle) * up, 0.0f);
    worldCorners[1] = vtx.worldPos + float4((scale * cosAngle - scale * sinAngle) * right + (scale * sinAngle + scale * cosAngle) * up, 0.0f);
    worldCorners[2] = vtx.worldPos + float4((scale * cosAngle + scale * sinAngle) * right + (scale * sinAngle - scale * cosAngle) * up, 0.0f);
    worldCorners[3] = vtx.worldPos + float4((-scale * cosAngle + scale * sinAngle) * right + (-scale * sinAngle - scale * cosAngle) * up, 0.0f);

    // 뷰 및 프로젝션 변환 적용
    for (int i = 0; i < 4; i++)
    {
        output[i].position = mul(worldCorners[i], g_matView);
        output[i].position = mul(output[i].position, g_matProjection);
    }

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
    uint id = input.id;
    float waveTime = g_data[id].curTime;
    
    // 기존 UV에 시간에 따른 파동 효과 적용
    float2 waveUV = input.uv;
    
    // 중심에서의 거리
    float distFromCenter = length(input.uv - float2(0.5f, 0.5f)) * 2.0f;
    
    // 동심원 파동 효과 (시간에 따라 변화)
    float waveStrength = 0.03f * sin(distFromCenter * 10.0f - waveTime * 2.0f);
    waveStrength *= (1.0f - distFromCenter * 0.5f);
    
    // 나선형 파동 효과
    float angle = atan2(input.uv.y - 0.5f, input.uv.x - 0.5f);
    float spiralWave = 0.02f * sin(angle * 5.0f + waveTime * 1.5f);
    
    // UV 좌표 왜곡
    waveUV += float2(waveStrength * cos(angle), waveStrength * sin(angle));
    waveUV += spiralWave * float2(cos(angle + 1.57f), sin(angle + 1.57f));
    
    // 왜곡된 UV로 파티클 텍스처 샘플링
    float4 particleColor = g_textures.Sample(g_sam_0, waveUV);
    
    // 가장자리 발광 효과
    float edgeGlow = 1.0f - smoothstep(0.8f, 1.0f, distFromCenter);
    float pulsingEdge = edgeGlow * (0.8f + 0.2f * sin(waveTime * 2.0f));
    
    float4 portalEdgeColor = input.color * pulsingEdge;
    float4 finalColor = particleColor;
    
	if (distFromCenter < 0.85f)
	{
		float distortAmount = 0.15f + 0.08f * sin(g_data[input.id].curTime * 5.0f);
		distortAmount += 0.03f * sin(waveTime * 0.7f + distFromCenter * 4.0f);

		float2 refractedUV = waveUV + (particleColor.rg - 0.5f) * distortAmount;
		refractedUV = saturate(refractedUV);

		float4 backgroundColor = g_textures2.Sample(g_sam_0, refractedUV);

		float waveR = 1.0f + 0.1f * sin(waveTime * 0.3f);
		float waveG = 0.7f + 0.1f * sin(waveTime * 0.5f);
		float waveB = 0.0f + 0.1f * sin(waveTime * 0.7f);
		float waveA = 0.3f + 0.05f * sin(waveTime);

		float appearT = saturate(g_data[input.id].curTime / 5.f);
		float mixRatio = saturate((g_data[input.id].curTime - 1.f) / 1.0f); 

		float4 portalColor = float4(waveR, waveG, waveB, waveA * appearT);

		float4 blended = lerp(portalColor, backgroundColor, mixRatio);

		return blended;
	}
    else
    {
        // 가장자리 발광 효과 추가
        return finalColor * (1.0f + 0.5f * pulsingEdge);
    }
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

    if (g_particle[threadIndex.x].alive == 0)
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
            
            float angle = 2.f;
            float radius = 5.0f;
            
            float3 position = float3(radius * cos(angle), radius * sin(angle), 0.0f);
            
            g_particle[threadIndex.x].worldPos = position;
            g_particle[threadIndex.x].worldDir = normalize(float3(cos(angle), sin(angle), 1.0f));
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * r2) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f;
            
            // Set initial rotation angle based on position in circle
            g_particle[threadIndex.x].rotationAngle = 0.0f;
        }
    }
    else
    {
        g_particle[threadIndex.x].curTime += deltaTime;
        
        // 회전
        /*
        if (g_particle[threadIndex.x].curTime >= g_particle[threadIndex.x].lifeTime)
        {
            g_particle[threadIndex.x].alive = 0;
        }
        else
        {
            // Rotate particles over time to create spinning portal effect
            g_particle[threadIndex.x].rotationAngle += deltaTime * 2.0f;
            
            // Calculate current progress ratio of lifetime
            float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
            
            // Portal opening animation: radius increases over time then stabilizes
            float initialRadius = 2.0f;
            float finalRadius = 5.0f;
            float currentRadius;
            
            if (ratio < 0.3f)
            {
                // Opening phase
                currentRadius = lerp(initialRadius, finalRadius, ratio / 0.3f);
            }
            else
            {
                // Stable phase
                currentRadius = finalRadius;
            }
            
            // Keep particles in ring formation with updated radius
            float angle = g_particle[threadIndex.x].rotationAngle;
            g_particle[threadIndex.x].worldPos.x = currentRadius * cos(angle);
            g_particle[threadIndex.x].worldPos.y = currentRadius * sin(angle);
        }
        */
    }
}

#endif