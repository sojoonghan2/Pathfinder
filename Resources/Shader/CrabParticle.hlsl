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
};

// ��ƼŬ�� ���� ���� ������ ���� ũ�⸦ ����ϰ�, ���� ���� ���� ǥ��
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

    // ��ƼŬ ���� ���� ����� ���
	float ratio = g_data[id].curTime / g_data[id].lifeTime;
    
    // �ð��� ���� ũ�� ��ȭ - �ʱ⿡ �� �� �۰� ����
	float scaleProfile = sin(ratio * 3.14159f) * 0.5f + 0.3f;
	float scale = lerp(g_float_0, g_float_1, scaleProfile) / 3.f; // ũ�⸦ 3.5�� ����
    
    // ��ƼŬ ���⿡ ���� �ణ�� �þ�� ȿ��
	float3 dir = normalize(g_data[id].worldDir);
	float stretchFactor = 1.0f + 0.5f * (1.0f - ratio); // �þ ȿ�� ����
    
    // View Space���� ��ƼŬ ���� ��� - ����� ���� ����
	float4 viewDir = mul(float4(dir, 0.0f), g_matView);
	float2 stretchDir = normalize(viewDir.xy) * scale * stretchFactor * 0.8f; // �þ ����
	float2 perpDir = float2(-stretchDir.y, stretchDir.x) * scale * 0.7f; // �� ������ ������
    
    // View Space���� ��ƼŬ ������ ����
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

    // ��� ������ lifeRatio ����
	output[0].lifeRatio = ratio;
	output[1].lifeRatio = ratio;
	output[2].lifeRatio = ratio;
	output[3].lifeRatio = ratio;
    
    // ���� ���� ����
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
    // �⺻ �ؽ�ó ����
	float4 texColor = g_textures.Sample(g_sam_0, input.uv);
    
    // ���� �ֱ⿡ ���� ���İ� ���� - ������ ��Ÿ���� õõ�� ���������
	float alphaProfile = (input.lifeRatio < 0.2f)
        ? smoothstep(0.0f, 0.2f, input.lifeRatio)
        : smoothstep(1.0f, 0.2f, input.lifeRatio);
    
    // ���� ���̵�ƿ� ȿ��
	float2 centeredUV = input.uv - 0.5f;
	float dist = length(centeredUV);
    
    // ���� ����� ���� ������ ���� �����ڸ� ó��
	float edgeFalloff = smoothstep(0.45f, 0.25f, dist);
    
    // ���� ���� - �� ���� �������� ����
	float3 brightBlood = float3(0.65f, 0.03f, 0.03f); // �� ���� ������
	float3 darkBlood = float3(0.2f, 0.01f, 0.01f); // ��ο� ������
    
    // �߾Ӻδ� ������, �����ڸ��� ��Ӱ�
	float3 baseBloodColor = lerp(brightBlood, darkBlood, dist * 1.2f);
    
    // �ð��� �������� �ణ ��ο����� ȿ��
	float3 finalBloodColor = lerp(baseBloodColor, darkBlood, input.lifeRatio * 0.3f);
    
    // ������ ��ȭ ȿ�� (�̹��� ���� ����)
	float noise = frac(sin(dot(input.uv + input.lifeRatio, float2(12.9898f, 78.233f))) * 43758.5453f);
	finalBloodColor *= (0.95f + 0.1f * noise); // ��ȭ �� ����
    
    // ���� ���İ� ��� - �� �����ϰ�
	float finalAlpha = texColor.a * alphaProfile * edgeFalloff * 0.85f;
    
    // �� �۰� ���� ���� ǥ���� ���� ����
	if (dist > 0.35f)
	{
		finalAlpha *= 0.6f; // �����ڸ� �� �����ϰ�
	}
    
	return float4(finalBloodColor, finalAlpha);
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
	int particleType = g_int_3;
	float deltaTime = g_vec2_1.x;
	float accTime = g_vec2_1.y;
	float minLifeTime = g_vec4_0.x;
	float maxLifeTime = g_vec4_0.y;
	float minSpeed = g_vec4_0.z;
	float maxSpeed = g_vec4_0.w;
    
    // Ȱ��ȭ ������ ��ƼŬ �� ����
	g_shared[0].addCount = addCount;
	GroupMemoryBarrierWithGroupSync();

    // ��Ȱ�� ��ƼŬ Ȱ��ȭ
	if (g_particle[threadIndex.x].alive == 0 && g_shared[0].addCount > 0)
	{
        // Ȱ��ȭ ������ ��ƼŬ �� ����
		while (true)
		{
			int remaining = g_shared[0].addCount;
			if (remaining <= 0)
				break;

			int expected = remaining;
			int desired = remaining - 1;
			int originalValue;
            
            // ���������� addCount �� ����
			InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);
            
			if (originalValue == expected)
			{
				g_particle[threadIndex.x].alive = 1;
				break;
			}
		}

        // ���� ��ƼŬ ���� - ���� ����� ���� ����
		if (g_particle[threadIndex.x].alive == 1)
		{
			float x = ((float) threadIndex.x / (float) maxCount) + accTime;

			float r1 = Rand(float2(x, accTime));
			float r2 = Rand(float2(x * accTime, accTime));
			float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));

            // ���� ����
			float3 noise =
			{
				2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1
			};

            // ���� �л� ���� - �� �а� �л�ǵ���
			float3 dir;
            
            // �پ��� �������� Ƣ�� �����
			dir.x = (noise.x - 0.5f) * 1.8f; // �¿�� �� �а� ����
			dir.y = noise.y * 0.9f - 0.3f; // �ణ �Ʒ��� (�߷�)
			dir.z = (noise.z - 0.5f) * 1.8f; // ���ķ� �� �а� ����
            
			g_particle[threadIndex.x].worldDir = normalize(dir);
            
            // �ʱ� ��ġ ���� - �� ���ߵ� �������� ����
			g_particle[threadIndex.x].worldPos = (noise.xyz - 0.5f) * 3.0f;
            
            // �پ��� ũ���� ����� (�� ���� ���� ����)
			float sizeVariance = pow(r1, 1.5f); // �� ���� ��ƼŬ ���� ����
			float lifeVariance = 1.0f - sizeVariance * 0.5f; // ���� ���ڴ� ���� ���� ��Ƴ���
            
			g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * lifeVariance) + minLifeTime;
			g_particle[threadIndex.x].curTime = 0.f;
		}
	}
    // ���� ��ƼŬ ������Ʈ
	else if (g_particle[threadIndex.x].alive == 1)
	{
        // ���� �������� deltaTime�� ���� ��ƼŬ�� ��� �ð� ������Ʈ
		g_particle[threadIndex.x].curTime += deltaTime;
		if (g_particle[threadIndex.x].lifeTime < g_particle[threadIndex.x].curTime)
		{
            // lifeTime���� curTime�� Ŀ���� ��ƼŬ ��Ȱ��ȭ
			g_particle[threadIndex.x].alive = 0;
			return;
		}

        // ���� ��� �ð� ����
		float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
        
        // �ӵ� �ް��� ���� - ������ ���� ǥ��
		float speedMultiplier = 1.0f - pow(ratio, 1.5f);
		float speed = (maxSpeed - minSpeed) * speedMultiplier + minSpeed * 0.15f;
        
        // �߷� ȿ�� - ��ƼŬ�� �ð��� ���� �Ʒ��� ������
		float3 gravity = float3(0.0f, -12.0f, 0.0f) * ratio * 1.5f;
        
        // ��ƼŬ ��ġ ������Ʈ
		g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
		g_particle[threadIndex.x].worldPos += gravity * deltaTime;
	}
}