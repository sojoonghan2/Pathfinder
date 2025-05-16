#include "ShaderParams.hlsl"
#include "utils.hlsl"

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
    float ratio = g_data[input.id].curTime / g_data[input.id].lifeTime;
    
    // ���� ��ȭ
    float3 startColor = float3(1.0f, 0.9f, 0.6f);
    float3 endColor = float3(0.5f, 0.3f, 0.1f);
    float3 color = lerp(startColor, endColor, ratio);
    
    // �ؽ�ó ����
    float4 texColor = g_textures.Sample(g_sam_0, input.uv);
    
    // Glow ȿ��
    float glow = sin(10.0f) * 0.5f + 0.5f;
    color += glow * float3(1.2f, 1.0f, 0.6f);
    
    // ���İ� ��� ����
    float fadeIn = smoothstep(0.0f, 0.2f, ratio);
    float fadeOut = 1.0f - smoothstep(0.8f, 1.0f, ratio);
    
    // ���� ���� �ϰ� ���� ����
    float alpha = fadeIn * fadeOut * texColor.a;
    
    return float4(color * texColor.rgb, alpha);
}

struct ComputeShared
{
    int addCount;
    float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

[numthreads(1024, 1, 1)]
void CS_Main(uint3 threadIndex : SV_DispatchThreadID)
{
    int id = threadIndex.x;
    if (id >= g_int_0)
        return;

    // ���ο� ���� �߰�
    if (g_particle[id].alive == 0)
    {
        g_particle[id].alive = 1;
            
        // �ʱ� ��ġ ���� (���� ������ ����)
        g_particle[id].worldPos = float3(0.0f, 0.0f, 0.0f);

        // ���� ������ �ӵ� ����
        float angle = Rand(float2(id, g_float_0)) * 3.141592 * 2.0f;
        float speed = g_float_1 * 2.0f + Rand(float2(id, g_float_1));

        g_particle[id].curTime = 0.0f;
        g_particle[id].lifeTime = g_float_1 + Rand(float2(id, g_float_2)) * 2.0f;
    }

    // ���� ���� ������Ʈ
    if (g_particle[id].alive == 1)
    {
        g_particle[id].curTime += g_vec2_1.x;
        
        // ������ ���ϸ� ����
        if (g_particle[id].curTime >= g_particle[id].lifeTime)
            g_particle[id].alive = 0;
    }
}