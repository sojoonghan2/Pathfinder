#include "ShaderParams.hlsl"
#include "utils.hlsl"

struct Particle
{
	float3 worldPos;
	float curTime;
	float3 worldDir;
	float lifeTime;
	int alive;
	float3 padding;
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
};

// 파티클의 현재 수명 비율에 따라 크기를 계산하고, 작은 혈흔 방울로 표현
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
    
    // 시간에 따른 크기 변화 - 초기에 좀 더 작게 시작
	float scaleProfile = sin(ratio * 3.14159f) * 0.5f + 0.3f;
	float scale = lerp(g_float_0, g_float_1, scaleProfile) / 3.f; // 크기를 3.5배 줄임
    
    // 파티클 방향에 따라 약간만 늘어나는 효과
	float3 dir = normalize(g_data[id].worldDir);
	float stretchFactor = 1.0f + 0.5f * (1.0f - ratio); // 늘어남 효과 감소
    
    // View Space에서 파티클 정점 계산 - 혈방울 형태 유지
	float4 viewDir = mul(float4(dir, 0.0f), g_matView);
	float2 stretchDir = normalize(viewDir.xy) * scale * stretchFactor * 0.8f; // 늘어남 감소
	float2 perpDir = float2(-stretchDir.y, stretchDir.x) * scale * 0.7f; // 더 원형에 가깝게
    
    // View Space에서 파티클 기하학 생성
	output[0].position = vtx.viewPos + float4(-perpDir - stretchDir * 0.8f, 0.f, 0.f);
	output[1].position = vtx.viewPos + float4(perpDir - stretchDir * 0.8f, 0.f, 0.f);
	output[2].position = vtx.viewPos + float4(perpDir + stretchDir * 0.8f, 0.f, 0.f);
	output[3].position = vtx.viewPos + float4(-perpDir + stretchDir * 0.8f, 0.f, 0.f);

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

    // 모든 정점에 lifeRatio 전달
	output[0].lifeRatio = ratio;
	output[1].lifeRatio = ratio;
	output[2].lifeRatio = ratio;
	output[3].lifeRatio = ratio;
    
    // 방향 정보 전달
	output[0].worldDir = dir;
	output[1].worldDir = dir;
	output[2].worldDir = dir;
	output[3].worldDir = dir;

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
    // 기본 텍스처 색상
	float4 texColor = g_textures.Sample(g_sam_0, input.uv);
    
    // 생명 주기에 따른 알파값 조절 - 빠르게 나타나고 천천히 사라지도록
	float alphaProfile = (input.lifeRatio < 0.2f)
        ? smoothstep(0.0f, 0.2f, input.lifeRatio)
        : smoothstep(1.0f, 0.2f, input.lifeRatio);
    
    // 원형 페이드아웃 효과
	float2 centeredUV = input.uv - 0.5f;
	float dist = length(centeredUV);
    
    // 작은 혈방울 형태 강조를 위한 가장자리 처리
	float edgeFalloff = smoothstep(0.45f, 0.25f, dist);
    
    // 혈흔 색상 - 더 진한 혈색으로 조정
	float3 brightBlood = float3(0.65f, 0.03f, 0.03f); // 덜 밝은 붉은색
	float3 darkBlood = float3(0.2f, 0.01f, 0.01f); // 어두운 붉은색
    
    // 중앙부는 붉은색, 가장자리는 어둡게
	float3 baseBloodColor = lerp(brightBlood, darkBlood, dist * 1.2f);
    
    // 시간이 지날수록 약간 어두워지는 효과
	float3 finalBloodColor = lerp(baseBloodColor, darkBlood, input.lifeRatio * 0.3f);
    
    // 무작위 변화 효과 (미묘한 혈흔 질감)
	float noise = frac(sin(dot(input.uv + input.lifeRatio, float2(12.9898f, 78.233f))) * 43758.5453f);
	finalBloodColor *= (0.95f + 0.1f * noise); // 변화 폭 감소
    
    // 최종 알파값 계산 - 더 투명하게
	float finalAlpha = texColor.a * alphaProfile * edgeFalloff * 0.85f;
    
    // 더 작고 옅은 혈흔 표현을 위한 조정
	if (dist > 0.35f)
	{
		finalAlpha *= 0.6f; // 가장자리 더 투명하게
	}
    
	return float4(finalBloodColor, finalAlpha);
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
	int particleType = g_int_3;
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

        // 혈흔 파티클 연산 - 작은 혈방울 생성 패턴
		if (g_particle[threadIndex.x].alive == 1)
		{
			float x = ((float) threadIndex.x / (float) maxCount) + accTime;

			float r1 = Rand(float2(x, accTime));
			float r2 = Rand(float2(x * accTime, accTime));
			float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));

            // 방향 난수
			float3 noise =
			{
				2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1
			};

            // 혈흔 분사 방향 - 더 넓게 분산되도록
			float3 dir;
            
            // 다양한 방향으로 튀는 혈방울
			dir.x = (noise.x - 0.5f) * 1.8f; // 좌우로 더 넓게 퍼짐
			dir.y = noise.y * 0.9f - 0.3f; // 약간 아래쪽 (중력)
			dir.z = (noise.z - 0.5f) * 1.8f; // 전후로 더 넓게 퍼짐
            
			g_particle[threadIndex.x].worldDir = normalize(dir);
            
            // 초기 위치 설정 - 더 집중된 지점에서 시작
			g_particle[threadIndex.x].worldPos = (noise.xyz - 0.5f) * 3.0f;
            
            // 다양한 크기의 혈방울 (더 많은 작은 방울들)
			float sizeVariance = pow(r1, 1.5f); // 더 작은 파티클 비율 증가
			float lifeVariance = 1.0f - sizeVariance * 0.5f; // 작은 입자는 비교적 오래 살아남음
            
			g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * lifeVariance) + minLifeTime;
			g_particle[threadIndex.x].curTime = 0.f;
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
        
        // 속도 급격히 감소 - 혈액의 점도 표현
		float speedMultiplier = 1.0f - pow(ratio, 1.5f);
		float speed = (maxSpeed - minSpeed) * speedMultiplier + minSpeed * 0.15f;
        
        // 중력 효과 - 파티클이 시간에 따라 아래로 떨어짐
		float3 gravity = float3(0.0f, -12.0f, 0.0f) * ratio * 1.5f;
        
        // 파티클 위치 업데이트
		g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
		g_particle[threadIndex.x].worldPos += gravity * deltaTime;
	}
}