#ifndef _GLITTERPARTICLE_FX_
#define _GLITTERPARTICLE_FX_

#include "params.fx"
#include "utils.fx"

#define PI 3.141592

struct Particle
{
    float3 worldPos;
    float curTime;
    float3 worldDir;
    float lifeTime;
    int alive;
    float rotation; // ȸ�� ���� �߰�
    float2 padding;
};

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

    // ȸ�� ��� ����
    float rotation = g_data[id].rotation;
    float cosTheta = cos(rotation);
    float sinTheta = sin(rotation);
    
    // View Space���� ȸ�� ����
    float4 positions[4];
    positions[0] = vtx.viewPos + float4(-scale, scale, 0.f, 0.f);
    positions[1] = vtx.viewPos + float4(scale, scale, 0.f, 0.f);
    positions[2] = vtx.viewPos + float4(scale, -scale, 0.f, 0.f);
    positions[3] = vtx.viewPos + float4(-scale, -scale, 0.f, 0.f);

    // ȸ�� ����
    for (int i = 0; i < 4; i++)
    {
        float x = positions[i].x;
        float y = positions[i].y;
        positions[i].x = x * cosTheta - y * sinTheta;
        positions[i].y = x * sinTheta + y * cosTheta;
    }

    // ���� ��ġ ����
    output[0].position = mul(positions[0], g_matProjection);
    output[1].position = mul(positions[1], g_matProjection);
    output[2].position = mul(positions[2], g_matProjection);
    output[3].position = mul(positions[3], g_matProjection);

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

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
    return g_textures.Sample(g_sam_0, input.uv);
}

struct ComputeShared
{
    int addCount;
    float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.x >= g_int_0)
        return;

    // �簢�� ����� �ʱ� ���� ����
    float spawnWidth = 50.0f; // �簢���� ���� ũ��
    float spawnDepth = 50.0f; // �簢���� ���� ũ��

    int maxCount = g_int_0;
    int addCount = g_int_1;
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
            float r4 = Rand(float2(x * r1, r2));

            // �簢�� ���� ������ ���� ��ġ ����
            g_particle[threadIndex.x].worldPos = float3(
                (r1 - 0.5f) * spawnWidth, // X: -spawnWidth/2 ~ spawnWidth/2
                0.0f, // Y: ���� ����
                (r2 - 0.5f) * spawnDepth // Z: -spawnDepth/2 ~ spawnDepth/2
            );

            // ��� ���⿡ �ణ�� ������ �߰�
            float spreadAngle = PI / 6.0f; // 30��
            float randomAngleX = (r3 - 0.5f) * spreadAngle;
            float randomAngleZ = (r4 - 0.5f) * spreadAngle;
            
            g_particle[threadIndex.x].worldDir = normalize(float3(
                sin(randomAngleX),
                1.0f, // �ַ� �������� ���ϰ�
                sin(randomAngleZ)
            ));

            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * r2) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.0f;
            g_particle[threadIndex.x].rotation = 0.0f; // �ʱ� ȸ�� ����
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
        float speed = (maxSpeed - minSpeed) * (1.0f - ratio) + minSpeed;

        // ��ġ ������Ʈ
        g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;

        // ȸ�� ������Ʈ (�ʴ� 2�� = 360�� ȸ��)
        g_particle[threadIndex.x].rotation += 2.0f * PI * deltaTime;

        // Y�� ������: sin �Լ��� ����� �ε巯�� ���
        float amplitude = 1.0f;
        g_particle[threadIndex.x].worldPos.y += sin(PI * ratio) * amplitude * deltaTime;
    }
}

#endif