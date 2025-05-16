#include "ShaderParams.hlsl"
#include "utils.hlsl"

struct Particle
{
	float3 worldPos;
	float curTime;
	float3 worldDir;
	float lifeTime;
	int alive;
	float initialTemperature; // �ʱ� �µ� (��ƼŬ ���� �� �ൿ ������ ���)
	float size; // ��ƼŬ ũ��
	float density; // ��ƼŬ �е� (������ �ൿ ������ ���)
};

// ����ȭ�� ���� ����
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

// ����ź ���� ��ƼŬ�� ���� ������Ʈ�� ���̴�
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

	// ��ü ũ�⸦ 0.7�� ���
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
    // �⺻ �ؽ�ó ����
	float4 texColor = g_textures.Sample(g_sam_0, input.uv);
    
    // ��ƼŬ Ư���� ���� ���� �� ���� ó��
	float3 finalColor;
	float finalAlpha;
	float ratio = input.lifeRatio;
	float initialTemp = input.initialTemp;
	float density = input.density;
	
	// ���� ���̵�ƿ� ȿ���� ���� �Ÿ� ���
	float2 centeredUV = input.uv - 0.5f;
	float dist = length(centeredUV);
	
	// �µ� ��� ���� ����
	float3 finalBaseColor;
	float3 coreColor = float3(1.0f, 0.9f, 0.5f); // �ſ� ���� ����� (���)
	float3 midColor = float3(1.0f, 0.5f, 0.0f); // ��Ȳ�� (�߰� �µ�)
	float3 edgeColor = float3(0.9f, 0.2f, 0.0f); // ������ (���� �µ�)
	float3 smokeColor = float3(0.3f, 0.3f, 0.3f); // ȸ�� (����)
	
	// �µ��� �е��� ���� ���� ����
	if (initialTemp > 0.8f) // ��� ��ƼŬ (ȭ��)
	{
		// �߾ӿ��� �����ڸ��� ���� ��ȭ
		if (dist < 0.3f)
			finalBaseColor = lerp(coreColor, midColor, dist / 0.3f);
		else
			finalBaseColor = lerp(midColor, edgeColor, (dist - 0.3f) / 0.7f);
		
		// �ð��� ���� ȭ�� ������ �Ӱ� ����
		finalColor = lerp(finalBaseColor, edgeColor, ratio * 0.8f);
		
		// ������ �����ǰ� õõ�� �����
		float alphaProfile = (ratio < 0.2f)
			? smoothstep(0.0f, 0.2f, ratio)
			: smoothstep(1.0f, 0.3f, ratio);
		
		// �����ڸ� ���̵�ƿ�
		float edgeFalloff = smoothstep(0.5f, 0.2f, dist);
		finalAlpha = texColor.a * alphaProfile * edgeFalloff * 0.9f;
	}
	else if (initialTemp > 0.3f) // �߰� �µ� ��ƼŬ (���� ���� ȥ��)
	{
		// �ʱ⿡�� ��ο� ��Ȳ��, �ð��� �����鼭 ��������� ����
		float3 initialColor = lerp(midColor, smokeColor, density);
		float3 finalSmokeColor = lerp(smokeColor, float3(0.7f, 0.7f, 0.7f), ratio);
		
		// �ð��� �������� ��������� ����
		finalColor = lerp(initialColor, finalSmokeColor, ratio);
		
		// õõ�� �����ǰ� õõ�� �����
		float alphaProfile = sin(ratio * 3.14159f);
		
		// �����ڸ��� �� �����ϰ�
		float edgeFalloff = smoothstep(0.5f, 0.0f, dist);
		finalAlpha = texColor.a * alphaProfile * edgeFalloff * 0.7f;
	}
	else // ���� ��ƼŬ (����)
	{
		// ������ ���� ��Ȳ������ ��ο� ȸ������
		float3 hotColor = lerp(edgeColor, float3(0.6f, 0.3f, 0.0f), density);
		float3 coolColor = lerp(float3(0.5f, 0.5f, 0.5f), float3(0.1f, 0.1f, 0.1f), density);
		
		// �ð��� �������� �ľ
		finalColor = lerp(hotColor, coolColor, ratio);
		
		// ó������ ��� �����ٰ� �����
		float glowFactor = 1.0f - ratio;
		finalColor += float3(0.3f * initialTemp, 0.2f * initialTemp, 0.0f) * glowFactor * glowFactor;
		
		// ó������ ������ ���̴ٰ� ������ �����
		float alphaProfile = 1.0f - smoothstep(0.7f, 1.0f, ratio);
		finalAlpha = alphaProfile;
	}
	
	// �ణ�� ���� ��ȭ�� �ڿ������� �߰� (��ƼŬ���� ������ ��)
	float noise = frac(sin(dot(input.uv + ratio + initialTemp, float2(12.9898f, 78.233f))) * 43758.5453f);
	finalColor *= (0.9f + 0.2f * noise);
	
	return float4(finalColor, finalAlpha);
}

struct ComputeShared
{
	int addCount;
	float3 padding;
};

// ��ǻƮ ���̴� ��/���
RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

// CS_Main
// g_vec2_1 : ��Ÿ �ð� / ���� �ð�
// g_int_0  : �ִ� ��ƼŬ ��
// g_int_1  : ���� Ȱ��ȭ�� ��ƼŬ ��
// g_vec4_0 : �ּ�/�ִ� ����� �ӵ�
[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
	if (threadIndex.x >= g_int_0)
		return;

	int maxCount = g_int_0;
	int addCount = g_int_1;
	int frameNumber = g_int_2;
	int explodeStrength = g_int_3;
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
			float r1 = Rand(float2(x, accTime));
			float r2 = Rand(float2(x * accTime, accTime));
			float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));
			float r4 = Rand(float2(x + accTime, x * accTime));
			float r5 = Rand(float2(x * x + accTime, accTime * x));

			float3 noise = float3(2 * r1 - 1, 2 * r2 - 1, 2 * r3 - 1);

			float temperature = pow(r4, 0.7f);
			float density = pow(r5, 1.5f);
			g_particle[threadIndex.x].initialTemperature = temperature;
			g_particle[threadIndex.x].density = density;

			float3 dir;
			float speedMultiplier;

			// �Ʒ����� ���� ������ ���⼺�� ��ȭ
			float3 upwardBias = float3(0.0f, 1.0f, 0.0f);
			float3 spreadDir = normalize(noise);
			float blend = (temperature > 0.8f) ? 0.6f : (temperature > 0.3f ? 0.4f : 0.2f);
			dir = normalize(lerp(spreadDir, upwardBias, blend));

			if (temperature > 0.8f)
				speedMultiplier = 0.8f + r1 * 0.3f;
			else if (temperature > 0.3f)
				speedMultiplier = 0.5f + r1 * 0.3f;
			else
				speedMultiplier = 1.0f + r1;

			g_particle[threadIndex.x].worldDir = dir * speedMultiplier;

			float initialOffset = r1 * 0.5f;
			g_particle[threadIndex.x].worldPos = dir * initialOffset;

			float lifeVariance;
			float sizeVariance;

			if (temperature > 0.8f)
			{
				lifeVariance = 0.3f + r2 * 0.2f;
				sizeVariance = 0.8f + r2 * 0.4f;
			}
			else if (temperature > 0.3f)
			{
				lifeVariance = 0.8f + r2 * 0.2f;
				sizeVariance = 0.6f + r2 * 0.8f;
			}
			else
			{
				lifeVariance = 0.5f + r2 * 0.3f;
				sizeVariance = 0.2f + r2 * 0.3f;
			}

			g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * lifeVariance) + minLifeTime;
			g_particle[threadIndex.x].curTime = 0.f;
			g_particle[threadIndex.x].size = sizeVariance * (1.0f + explodeStrength * 0.1f);
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
		float temp = g_particle[threadIndex.x].initialTemperature;
		float density = g_particle[threadIndex.x].density;

		if (temp > 0.8f)
		{
			float speedMultiplier = 1.0f - pow(ratio, 2.0f);
			float speed = (maxSpeed - minSpeed) * speedMultiplier + minSpeed * 0.2f;
			float3 buoyancy = float3(0.0f, 5.0f, 0.0f) * ratio;
			g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
			g_particle[threadIndex.x].worldPos += buoyancy * deltaTime;
		}
		else if (temp > 0.3f)
		{
			float speedMultiplier = 1.0f - ratio * 0.7f;
			float speed = (maxSpeed * 0.3f - minSpeed) * speedMultiplier + minSpeed;
			float3 buoyancy = float3(
				sin(ratio * 6.28f + g_particle[threadIndex.x].worldPos.x) * 0.5f,
				3.0f + sin(ratio * 3.14f) * 0.5f,
				cos(ratio * 6.28f + g_particle[threadIndex.x].worldPos.z) * 0.5f
			) * (1.0f - ratio * 0.5f);
			g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
			g_particle[threadIndex.x].worldPos += buoyancy * deltaTime;
		}
		else
		{
			float speedMultiplier = 1.0f - pow(ratio, 0.7f);
			float speed = (maxSpeed * 2.0f - minSpeed) * speedMultiplier + minSpeed;
			float3 gravity = float3(0.0f, -20.0f * density, 0.0f) * ratio;
			g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
			g_particle[threadIndex.x].worldPos += gravity * deltaTime;
		}
	}
}