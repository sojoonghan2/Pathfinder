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

// �׷��Ƚ� ���̴�
// StructuredBuffer ����
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

// ��ƼŬ�� ���� ���� ������ ���� ũ�⸦ ����ϰ�, ȭ�鿡 �簢�� ���·� Ȯ��
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

    // �⺻ ũ�� ����
    float baseScale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;

    // ������ ũ�� ����
    float scaleX = baseScale * (1.2f + (Rand(float2(id, ratio)) * 0.5f - 0.25f));
    float scaleY = baseScale * (1.2f + (Rand(float2(ratio, id)) * 0.5f - 0.25f));

    // ���� ȸ�� �߰� (�տ��� �ڿ������� ���̵���)
    float angle = Rand(float2(id, ratio)) * PI * 2.0f;
    float cosA = cos(angle);
    float sinA = sin(angle);

    // ���Ī�� �տ� ����� ���� �ٰ��� ���� ����
    float2 crackedShape[4] =
    {
        float2(-scaleX * 1.2f, scaleY * 0.8f),
        float2(scaleX * 0.9f, scaleY * 1.2f),
        float2(scaleX * 1.2f, -scaleY * 0.9f),
        float2(-scaleX * 0.8f, -scaleY * 1.1f)
    };

    // ȸ�� ����
    for (int i = 0; i < 4; i++)
    {
        crackedShape[i] = float2(
            crackedShape[i].x * cosA - crackedShape[i].y * sinA,
            crackedShape[i].x * sinA + crackedShape[i].y * cosA
        );
    }

    // View Space ����
    output[0].position = vtx.viewPos + float4(crackedShape[0], 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(crackedShape[1], 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(crackedShape[2], 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(crackedShape[3], 0.f, 0.f);

    // Projection ��ȯ
    for (int i = 0; i < 4; i++)
    {
        output[i].position = mul(output[i].position, g_matProjection);
    }

    // uv �� id ����
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    for (int i = 0; i < 4; i++)
    {
        output[i].id = id;
    }

    // �ﰢ�� ��Ʈ�� �߰�
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
    
    // �տ� �߽� ����
    float3 startColor = float3(0.2f, 0.2f, 0.2f); // £�� ȸ�� (�ٴ�)
    float3 endColor = float3(0.8f, 0.8f, 0.8f); // ��� (�μ����� ���)
    
    // �ε巯�� ���� ��ȯ
    float3 color = lerp(startColor, endColor, ratio);

    // �Ÿ� ��� ���İ� ����
    float2 center = float2(0.5f, 0.5f);
    float dist = distance(input.uv, center);

    float alpha = saturate(1.0f - dist * 2.0f); // �߽� 1.0, �����ڸ� 0.0
    alpha = pow(alpha, 2.5f); // �� �ε巯�� ����� ȿ��

    // �ؽ�ó ���ø� �� ������ ����
    float2 uvOffset = float2(Rand(float2(input.id, ratio)) * 0.1f - 0.05f,
                             Rand(float2(ratio, input.id)) * 0.1f - 0.05f);
    float2 sampledUV = saturate(input.uv + uvOffset);
    
    float4 texColor = g_textures.Sample(g_sam_0, sampledUV);

    return float4(color, alpha) * texColor;
}

struct ComputeShared
{
    int addCount;
    float3 padding;
};

// ��ǻƮ ���̴� ��/���
// ��ƼŬ �����Ͱ� ����� ����
RWStructuredBuffer<Particle> g_particle : register(u0);
// Ȱ��ȭ�� ��ƼŬ ���� �����ϴ� ���� ������
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

    // �ʱ� ���� ����
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

    // Ȱ��ȭ ������ ��ƼŬ �� ����
    g_shared[0].addCount = addCount;
    // ������ ����ȭ
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

            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);
            if (originalValue == expected)
            {
                g_particle[threadIndex.x].alive = 1;
                break;
            }
        }

        // ��ݺ� ��ƼŬ ����
        if (g_particle[threadIndex.x].alive == 1)
        {
            float x = ((float) threadIndex.x / (float) maxCount) + accTime;
            // �ﰢ�� ������ ����
            float2 A = float2(-300.0f, 500.0f);
            float2 B = float2(300.0f, 500.0f);
            float2 C = float2(0.0f, -50.0f);

            // ������ �ﰢ�� ������ ���� ������ �ٸ���Ʈ�� ��ǥ ����
            float r1 = Rand(float2(x, accTime));
            float r2 = Rand(float2(x * accTime, accTime));
    
            // ������ �ٸ���Ʈ�� ��ǥ ���
            float u = 1.0f - sqrt(r1);
            float v = r2 * sqrt(r1);
            float w = 1.0f - u - v;
    
            // �ﰢ�� ������ ���� ��ǥ ���
            float2 randPosition = u * A + v * B + w * C;
    
            g_particle[threadIndex.x].worldPos = float3(randPosition.x, 0.0f, randPosition.y);
            
            // ��ġ�� �������� ���� ����
            float randomSeed = Rand(float2(accTime, threadIndex.x));
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * randomSeed) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f;
        }
    }
    // ���� ��ƼŬ ������Ʈ
    else
    {
        // ���� �ð� ������Ʈ
        g_particle[threadIndex.x].curTime += deltaTime;

        // �տ��� ���� �о������� �ӵ��� �ٸ��� ����
        float progress = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;

        // �ʱ� �ӵ��� ������, ���� ���������� ����
        float3 velocity = g_particle[threadIndex.x].worldDir; // ������ �⺻ �̵� ����
        float initialSpeed = 5000.0f; // �ʱ� ���� �ӵ� (���� ���)
        float gravity = -5000.0f; // �߷� (������ �������� ����)
    
        // ������ �ٶ� ȿ�� ���� (X, Z ���� Ȯ��)
        float windFactor = 0.5f;
        float windX = (Rand(float2(progress, threadIndex.x)) - 0.5f) * windFactor;
        float windZ = (Rand(float2(threadIndex.x, progress)) - 0.5f) * windFactor;

        // �ʱ⿡�� ������ Ƣ�����
        if (progress < 0.01f)
        {
            velocity.y += initialSpeed * (1.0f - progress); // ���� ����
        }
        // �߷� ���� ����
        else if (progress > 0.01f)
        {
            velocity.y += gravity * deltaTime;
        }

        // X, Z ���� Ȯ�� (�ٶ� ȿ�� �߰�)
        velocity.x += windX * deltaTime;
        velocity.z += windZ * deltaTime;

        // ��ġ ������Ʈ
        g_particle[threadIndex.x].worldPos += velocity * deltaTime;

        // ������ ���� ��ƼŬ ����
        if (g_particle[threadIndex.x].curTime >= g_particle[threadIndex.x].lifeTime)
        {
            g_particle[threadIndex.x].alive = 0;
        }
    }
}

#endif