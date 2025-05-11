#ifndef _GRENADEPARTICLE_FX_
#define _GRENADEPARTICLE_FX_

#include "params.fx"
#include "utils.fx"

struct Particle
{
	float3 worldPos;
	float curTime;
	float3 worldDir;
	float lifeTime;
	int alive;
	float initialTemperature; // 초기 온도 (파티클 색상 및 행동 결정에 사용)
	float size; // 파티클 크기
	float density; // 파티클 밀도 (물리적 행동 결정에 사용)
};

// 구조화된 버퍼 정의
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
	float lifeRatio : TEXCOORD1;
	float3 worldDir : TEXCOORD2;
	float initialTemp : TEXCOORD3;
	float density : TEXCOORD4;
};

// 수류탄 폭발 파티클을 위한 지오메트리 셰이더
[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
	GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
	uint id = input[0].id;
	if (g_data[id].alive == 0)
		return;

	float ratio = g_data[id].curTime / g_data[id].lifeTime;
	float initialTemp = g_data[id].initialTemperature;
	float density = g_data[id].density;

	float scaleProfile;
	float scale = g_data[id].size;

	if (initialTemp > 0.8f)
		scaleProfile = sin(ratio * 3.14159f) * 0.6f + 0.2f;
	else if (initialTemp > 0.3f)
		scaleProfile = smoothstep(0.0f, 1.0f, ratio);
	else
		scaleProfile = 1.0f;

	// 전체 크기를 0.7배 축소
	scale *= lerp(g_float_0, g_float_1 * 0.8f, scaleProfile);

	float3 dir = normalize(g_data[id].worldDir);
	float stretchFactor = 1.0f;

	if (density > 0.7f && length(dir) > 0.5f)
		stretchFactor = 1.0f + 1.5f * length(dir) * density;

	float4 viewDir = mul(float4(dir, 0.0f), g_matView);
	float2 billboardDir;
	float2 perpDir;

	if (density > 0.7f)
	{
		billboardDir = normalize(viewDir.xy) * scale * stretchFactor;
		perpDir = float2(-billboardDir.y, billboardDir.x) * scale * (0.1f + 0.3f * (1.0f - density));
	}
	else
	{
		billboardDir = float2(0.0f, scale * 0.4f);
		perpDir = float2(scale * 0.8f, 0.0f);
	}

	float4 viewPos = input[0].viewPos;
	output[0].position = mul(viewPos + float4(-perpDir - billboardDir, 0.f, 0.f), g_matProjection);
	output[1].position = mul(viewPos + float4(perpDir - billboardDir, 0.f, 0.f), g_matProjection);
	output[2].position = mul(viewPos + float4(perpDir + billboardDir, 0.f, 0.f), g_matProjection);
	output[3].position = mul(viewPos + float4(-perpDir + billboardDir, 0.f, 0.f), g_matProjection);

	float2 uvs[4] = { float2(0, 0), float2(1, 0), float2(1, 1), float2(0, 1) };
	for (int i = 0; i < 4; ++i)
	{
		output[i].uv = uvs[i];
		output[i].id = id;
		output[i].lifeRatio = ratio;
		output[i].worldDir = dir;
		output[i].initialTemp = initialTemp;
		output[i].density = density;
	}

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
    // 기본 텍스처 색상
	float4 texColor = g_textures.Sample(g_sam_0, input.uv);
    
    // 파티클 특성에 따른 색상 및 알파 처리
	float3 finalColor;
	float finalAlpha;
	float ratio = input.lifeRatio;
	float initialTemp = input.initialTemp;
	float density = input.density;
	
	// 원형 페이드아웃 효과를 위한 거리 계산
	float2 centeredUV = input.uv - 0.5f;
	float dist = length(centeredUV);
	
	// 온도 기반 색상 선택
	float3 finalBaseColor;
	float3 coreColor = float3(1.0f, 0.9f, 0.5f); // 매우 밝은 노란색 (고온)
	float3 midColor = float3(1.0f, 0.5f, 0.0f); // 주황색 (중간 온도)
	float3 edgeColor = float3(0.9f, 0.2f, 0.0f); // 붉은색 (낮은 온도)
	float3 smokeColor = float3(0.3f, 0.3f, 0.3f); // 회색 (연기)
	
	// 온도와 밀도에 따른 색상 조합
	if (initialTemp > 0.8f) // 고온 파티클 (화염)
	{
		// 중앙에서 가장자리로 색상 변화
		if (dist < 0.3f)
			finalBaseColor = lerp(coreColor, midColor, dist / 0.3f);
		else
			finalBaseColor = lerp(midColor, edgeColor, (dist - 0.3f) / 0.7f);
		
		// 시간에 따라 화염 색상이 붉게 변함
		finalColor = lerp(finalBaseColor, edgeColor, ratio * 0.8f);
		
		// 빠르게 생성되고 천천히 사라짐
		float alphaProfile = (ratio < 0.2f)
			? smoothstep(0.0f, 0.2f, ratio)
			: smoothstep(1.0f, 0.3f, ratio);
		
		// 가장자리 페이드아웃
		float edgeFalloff = smoothstep(0.5f, 0.2f, dist);
		finalAlpha = texColor.a * alphaProfile * edgeFalloff * 0.9f;
	}
	else if (initialTemp > 0.3f) // 중간 온도 파티클 (열과 연기 혼합)
	{
		// 초기에는 어두운 주황색, 시간이 지나면서 연기색으로 변함
		float3 initialColor = lerp(midColor, smokeColor, density);
		float3 finalSmokeColor = lerp(smokeColor, float3(0.7f, 0.7f, 0.7f), ratio);
		
		// 시간이 지날수록 연기색으로 변함
		finalColor = lerp(initialColor, finalSmokeColor, ratio);
		
		// 천천히 생성되고 천천히 사라짐
		float alphaProfile = sin(ratio * 3.14159f);
		
		// 가장자리가 더 투명하게
		float edgeFalloff = smoothstep(0.5f, 0.0f, dist);
		finalAlpha = texColor.a * alphaProfile * edgeFalloff * 0.7f;
	}
	else // 저온 파티클 (파편)
	{
		// 파편은 밝은 주황색에서 어두운 회색으로
		float3 hotColor = lerp(edgeColor, float3(0.6f, 0.3f, 0.0f), density);
		float3 coolColor = lerp(float3(0.5f, 0.5f, 0.5f), float3(0.1f, 0.1f, 0.1f), density);
		
		// 시간이 지날수록 식어감
		finalColor = lerp(hotColor, coolColor, ratio);
		
		// 처음에는 밝게 빛나다가 사라짐
		float glowFactor = 1.0f - ratio;
		finalColor += float3(0.3f * initialTemp, 0.2f * initialTemp, 0.0f) * glowFactor * glowFactor;
		
		// 처음부터 완전히 보이다가 빠르게 사라짐
		float alphaProfile = 1.0f - smoothstep(0.7f, 1.0f, ratio);
		finalAlpha = alphaProfile;
	}
	
	// 약간의 랜덤 변화로 자연스러움 추가 (파티클마다 고유한 값)
	float noise = frac(sin(dot(input.uv + ratio + initialTemp, float2(12.9898f, 78.233f))) * 43758.5453f);
	finalColor *= (0.9f + 0.2f * noise);
	
	return float4(finalColor, finalAlpha);
}

struct ComputeShared
{
	int addCount;
	float3 padding;
};

// 컴퓨트 셰이더 입/출력
RWStructuredBuffer<Particle> g_particle : register(u0);
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
	int explodeStrength = g_int_3; // 폭발 강도
	float deltaTime = g_vec2_1.x;
	float accTime = g_vec2_1.y;
	float minLifeTime = g_vec4_0.x;
	float maxLifeTime = g_vec4_0.y;
	float minSpeed = g_vec4_0.z;
	float maxSpeed = g_vec4_0.w;
    
    // 활성화 가능한 파티클 수 관리
	g_shared[0].addCount = addCount;
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
            
            // 원자적으로 addCount 값 감소
			InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);
            
			if (originalValue == expected)
			{
				g_particle[threadIndex.x].alive = 1;
				break;
			}
		}

        // 수류탄 폭발 파티클 연산
		if (g_particle[threadIndex.x].alive == 1)
		{
			float x = ((float) threadIndex.x / (float) maxCount) + accTime;

			// 파티클마다 고유한 난수값 생성
			float r1 = Rand(float2(x, accTime));
			float r2 = Rand(float2(x * accTime, accTime));
			float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));
			float r4 = Rand(float2(x + accTime, x * accTime));
			float r5 = Rand(float2(x * x + accTime, accTime * x));

            // 방향 난수
			float3 noise =
			{
				2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1
			};
			
			// 파티클 속성 결정
			// 온도: 0(차가움) ~ 1(뜨거움)
			// 밀도: 0(가벼움) ~ 1(무거움)
			
			float temperature = pow(r4, 0.7f); // 온도 분포 (높은 온도가 많음)
			float density = pow(r5, 1.5f); // 밀도 분포 (낮은 밀도가 많음)
			
			// 파티클 특성 설정
			g_particle[threadIndex.x].initialTemperature = temperature;
			g_particle[threadIndex.x].density = density;

            // 파티클 특성에 따른 방향 및 속도 설정
			float3 dir;
			float speedMultiplier;
			
			if (temperature > 0.8f) // 고온 파티클 (화염)
			{
				// 주로 위쪽으로 확산
				dir.x = noise.x * 0.7f;
				dir.y = r2 * 0.7f + 0.3f; // 위쪽 방향 편향
				dir.z = noise.z * 0.7f;
				
				speedMultiplier = 0.7f + r1 * 0.3f;
			}
			else if (temperature > 0.3f) // 중간 온도 파티클 (열과 연기 혼합)
			{
				// 넓게 퍼지고 위로 올라감
				dir.x = noise.x;
				dir.y = r2 * 0.6f + 0.4f; // 위쪽 방향 강한 편향
				dir.z = noise.z;
				
				speedMultiplier = 0.4f + r1 * 0.3f; // 상대적으로 느림
			}
			else // 저온 파티클 (파편)
			{
				// 모든 방향으로 강하게 튀어나감
				dir = normalize(noise);
				speedMultiplier = 1.0f + r1; // 가장 빠름
			}
			
			g_particle[threadIndex.x].worldDir = normalize(dir) * speedMultiplier;
            
            // 폭발 중심에서 약간 랜덤하게 시작
			float initialOffset = r1 * 0.5f;
			g_particle[threadIndex.x].worldPos = normalize(noise) * initialOffset;
            
            // 파티클 수명 및 크기 설정
			float lifeVariance;
			float sizeVariance;
			
			if (temperature > 0.8f) // 고온 파티클 (화염)
			{
				lifeVariance = 0.3f + r2 * 0.2f;
				sizeVariance = 0.8f + r2 * 0.4f;
			}
			else if (temperature > 0.3f) // 중간 온도 파티클 (열과 연기 혼합)
			{
				lifeVariance = 0.8f + r2 * 0.2f;
				sizeVariance = 0.6f + r2 * 0.8f;
			}
			else // 저온 파티클 (파편)
			{
				lifeVariance = 0.5f + r2 * 0.3f;
				sizeVariance = 0.2f + r2 * 0.3f;
			}
            
			g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * lifeVariance) + minLifeTime;
			g_particle[threadIndex.x].curTime = 0.f;
			g_particle[threadIndex.x].size = sizeVariance * (1.0f + explodeStrength * 0.1f);
		}
	}
    // 기존 파티클 업데이트
	else if (g_particle[threadIndex.x].alive == 1)
	{
        // 현재 프레임의 deltaTime을 더해 파티클의 경과 시간 업데이트
		g_particle[threadIndex.x].curTime += deltaTime;
		if (g_particle[threadIndex.x].lifeTime < g_particle[threadIndex.x].curTime)
		{
            // lifeTime보다 curTime이 커지면 파티클 비활성화
			g_particle[threadIndex.x].alive = 0;
			return;
		}

        // 현재 경과 시간 비율
		float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
		float temp = g_particle[threadIndex.x].initialTemperature;
		float density = g_particle[threadIndex.x].density;
		
		// 파티클 특성별 움직임 연산
		if (temp > 0.8f) // 고온 파티클 (화염)
		{
			// 빠르게 감속, 약간 위로 이동
			float speedMultiplier = 1.0f - pow(ratio, 2.0f);
			float speed = (maxSpeed - minSpeed) * speedMultiplier + minSpeed * 0.2f;
			
			// 부력 효과 - 위로 올라감
			float3 buoyancy = float3(0.0f, 5.0f, 0.0f) * ratio;
			
			// 파티클 위치 업데이트
			g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
			g_particle[threadIndex.x].worldPos += buoyancy * deltaTime;
		}
		else if (temp > 0.3f) // 중간 온도 파티클 (열과 연기 혼합)
		{
			// 천천히 위로 올라가며 확산
			float speedMultiplier = 1.0f - ratio * 0.7f;
			float speed = (maxSpeed * 0.3f - minSpeed) * speedMultiplier + minSpeed;
			
			// 부력과 약간의 랜덤 움직임
			float3 buoyancy = float3(
				sin(ratio * 6.28f + g_particle[threadIndex.x].worldPos.x) * 0.5f,
				3.0f + sin(ratio * 3.14f) * 0.5f,
				cos(ratio * 6.28f + g_particle[threadIndex.x].worldPos.z) * 0.5f
			) * (1.0f - ratio * 0.5f);
			
			// 파티클 위치 업데이트
			g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
			g_particle[threadIndex.x].worldPos += buoyancy * deltaTime;
		}
		else // 저온 파티클 (파편)
		{
			// 초기 속도가 빠르고 중력 영향을 크게 받음
			float speedMultiplier = 1.0f - pow(ratio, 0.7f);
			float speed = (maxSpeed * 2.0f - minSpeed) * speedMultiplier + minSpeed;
			
			// 중력 효과 - 시간이 갈수록 강해짐 (밀도에 비례)
			float3 gravity = float3(0.0f, -20.0f * density, 0.0f) * ratio;
			
			// 파티클 위치 업데이트
			g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
			g_particle[threadIndex.x].worldPos += gravity * deltaTime;
		}
	}
}

#endif