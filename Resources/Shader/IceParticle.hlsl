#include "ShaderParams.hlsl"
#include "utils.hlsl"

struct Particle
{
    float3  worldPos;   // 12
    float   curTime;    // 4
    float3  worldDir;   // 12
    float   lifeTime;   // 4
    int     alive;      // 4
    float3 padding;     // 12
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
        (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f
    };

    VS_OUT vtx = input[0];
    uint id = (uint)vtx.id;
    if (0 == g_data[id].alive)
        return;

    // ��ƼŬ ���� ���� ����� ���
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

// PS_Main ���� - �����ֱ⿡ ���� ���� ��ȭ �߰�
float4 PS_Main(GS_OUT input) : SV_Target
{
    uint id = input.id;
    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    
    float4 color = g_textures.Sample(g_sam_0, input.uv);
    
    // ���� �ֱ⿡ ���� ���� ��ȯ
    if (ratio < 0.4f)
    {
        // �ʱ� �ܰ�: ���� �� ���� (���/��Ȳ)
        color *= float4(1.0f, 0.7f + 0.3f * (1 - ratio / 0.4f), 0.2f, 1.0f);
    }
    else if (ratio < 0.7f)
    {
        // �߰� �ܰ�: �ҿ��� ����� ��ȯ
        float transitionRatio = (ratio - 0.4f) / 0.3f;
        float3 fireColor = float3(1.0f, 0.7f, 0.2f);
        float3 smokeColor = float3(0.3f, 0.3f, 0.3f);
        color.rgb *= lerp(fireColor, smokeColor, transitionRatio);
    }
    else
    {
        // ������ �ܰ�: ���� �������� ���� ��������
        float fadeRatio = (ratio - 0.7f) / 0.3f;
        color *= float4(0.3f, 0.3f, 0.3f, 0.8f - fadeRatio);
    }
    
    return color;
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
            
            // ����������(�� �̻� �ɰ� �� ���� ����)�� addCount �� ����
            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);
            
            // ���������� ���� ���ҽ�Ų ������� �ش� ��ƼŬ�� Ȱ��ȭ(alive = 1)
            if (originalValue == expected)
            {
                g_particle[threadIndex.x].alive = 1;
                break;
            }
        }

        // ���� ��ƼŬ ����
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

            // ���� �ַ� �������� �ö󰡹Ƿ� Y���⿡ ����ġ�� ��
            float3 dir = normalize(float3(noise.x * 0.5f, 1.0f + noise.y * 0.3f, noise.z * 0.5f));
    
            // ��ƼŬ �ʱ� ��ġ�� ���� �ݰ� ������ �����ϰ� ����
            g_particle[threadIndex.x].worldDir = dir;
            g_particle[threadIndex.x].worldPos = float3((noise.x - 0.5f) * 10.0f, (noise.y - 0.5f) * 2.0f, (noise.z - 0.5f) * 10.0f);
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * r1) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f;
        }
        
    }
    // ���� ��ƼŬ ������Ʈ
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
    
        // ��ƼŬ ��� �ӵ��� �����ֱ⿡ ���� ����
        // �ʱ⿡�� ������ �ö󰡴ٰ� ���߿��� �� ������ ������ ȿ��
        float3 currentDir = g_particle[threadIndex.x].worldDir;
        if (ratio > 0.4f)
        {
            // ���� �ܰ迡���� �� ������ �����Ӱ� ���� ���
            float turbulence = sin(ratio * 10.0f + g_particle[threadIndex.x].worldPos.x) * 0.2f;
            currentDir = normalize(float3(
            currentDir.x + turbulence,
            currentDir.y * (1.0f - ratio * 0.7f), // Y ���� �ӵ� ����
            currentDir.z + turbulence
        ));
        }
    
        g_particle[threadIndex.x].worldPos += currentDir * speed * deltaTime;
    }
}