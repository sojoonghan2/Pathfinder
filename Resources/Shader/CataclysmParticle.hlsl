#ifndef _CATACLYSMPARTICLE_FX_
#define _CATACLYSMPARTICLE_FX_

#include "ShaderParams.fx"
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
    
    // ���൵�� ���� ���� ��ȭ
    float3 baseColor = float3(0.6f, 0.6f, 0.6f); // �⺻ ȸ��
    float3 startColor = float3(0.8f, 0.7f, 0.5f); // Ȳ���
    float3 endColor = float3(0.3f, 0.3f, 0.3f); // ��ο� ȸ��
    
    float3 color;
    if (ratio < 0.3f)
    {
        // �ʱ�: ���� ������ �⺻������
        color = lerp(startColor, baseColor, ratio / 0.3f);
    }
    else
    {
        // �ı�: �⺻������ ��ο� ������
        color = lerp(baseColor, endColor, (ratio - 0.3f) / 0.7f);
    }
    
    // �տ� ���� ����
    float2 center = float2(0.5f, 0.5f);
    float dist = distance(input.uv, center);
    
    // �ұ�Ģ�� �տ� ����
    float noise = Rand(float2(input.id, ratio)) * 0.2f;
    float crackPattern = saturate(1.0f - dist * (2.0f + noise));
    crackPattern = pow(crackPattern, 1.5f);
    
    float4 texColor = g_textures.Sample(g_sam_0, input.uv);
    
    // ���İ� ����
    float fadeFactor = smoothstep(0.5f, 1.0f, ratio); // 50%���� ������ ���̵�ƿ�
    float alpha = crackPattern * (1.0f - pow(fadeFactor, 2.0f)); // õõ�� ����
    
    // ���� ����
    return float4(color * texColor.rgb, alpha * texColor.a);
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

    int maxCount = g_int_0;
    int addCount = g_int_1;
    float deltaTime = g_vec2_1.x;
    float accTime = g_vec2_1.y;
    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;

    g_shared[0].addCount = addCount;
    GroupMemoryBarrierWithGroupSync();

    // ��Ȱ�� ��ƼŬ Ȱ��ȭ
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
            
            // ��ä�� ������� ��ƼŬ ���� (Z+ �������� ����)
            float radius = lerp(50.0f, 200.0f, Rand(float2(x, accTime)));
            float angle = lerp(-PI * 0.3f, PI * 0.3f, Rand(float2(x * accTime, accTime)));
            
            float posX = radius * sin(angle);
            float posZ = radius * cos(angle);
            
            g_particle[threadIndex.x].worldPos = float3(posX, 0.0f, posZ);
            
            // �ʱ� ���� ���� (Z+ �������� �ٱ�������)
            float3 direction = normalize(float3(posX,
                lerp(100.0f, 300.0f, Rand(float2(x, angle))), // ����
                posZ));
            
            // �ʱ� �ӵ� ����
            float initialVelocity = lerp(1000.0f, 2000.0f, Rand(float2(accTime, x)));
            g_particle[threadIndex.x].worldDir = direction * initialVelocity;
            
            // ���� ����
            g_particle[threadIndex.x].lifeTime = lerp(minLifeTime, maxLifeTime,
                Rand(float2(accTime, threadIndex.x)));
            g_particle[threadIndex.x].curTime = 0.f;
        }
    }
    // Ȱ�� ��ƼŬ ������Ʈ
    else if (g_particle[threadIndex.x].alive == 1)
    {
        g_particle[threadIndex.x].curTime += deltaTime;
        float progress = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;

        float3 velocity = g_particle[threadIndex.x].worldDir;
        
        // �߷� �� ���� ȿ��
        float gravity = -981.0f;
        float dragFactor = 1.0f;
        
        // ���൵�� ���� �߷� ���� ����
        if (progress < 0.2f)
        {
            // �ʱ⿡�� �߷� ���� ����
            velocity.y += gravity * deltaTime * 0.5f;
            dragFactor = 0.3f;
        }
        else
        {
            // ���� �߷� ���� ����
            velocity.y += gravity * deltaTime;
            dragFactor = lerp(0.3f, 1.0f, (progress - 0.2f) / 0.8f);
        }
        
        // ���� ����
        velocity *= (1.0f - dragFactor * deltaTime);
        
        // ���� �浹 ó��
        if (g_particle[threadIndex.x].worldPos.y < 0.0f)
        {
            g_particle[threadIndex.x].worldPos.y = 0.0f;
            velocity.y = -velocity.y * 0.4f; // ź�� ���
            velocity.xz *= 0.9f; // ����
        }
        
        g_particle[threadIndex.x].worldDir = velocity;
        g_particle[threadIndex.x].worldPos += velocity * deltaTime;

        if (g_particle[threadIndex.x].curTime >= g_particle[threadIndex.x].lifeTime * 0.95f)
        {
            float fadeProgress = (g_particle[threadIndex.x].curTime - g_particle[threadIndex.x].lifeTime * 0.95f)
                           / (g_particle[threadIndex.x].lifeTime * 0.05f);
            if (fadeProgress >= 1.0f)
            {
                g_particle[threadIndex.x].alive = 0;
            }
        }
    }
}

#endif