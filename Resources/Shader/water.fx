#ifndef _WATER_FX_
#define _WATER_FX_

#include "params.fx"
#include "utils.fx"

// ���� ���̴��� ���޵� �Է� ����ü
struct VS_IN
{
    float3 pos : POSITION; // ���� ��ġ
    float2 uv : TEXCOORD; // �ؽ�ó ��ǥ
    float3 normal : NORMAL; // ���� ���
    float3 tangent : TANGENT; // ���� ���� (Tangent)
};

// ���� ���̴����� �ȼ� ���̴��� ���޵Ǵ� ��� ����ü
struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewDir : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
};

// ���� ���̴�: ������ ��ȯ�ϰ� �ʿ��� �����͸� �ȼ� ���̴��� ����
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.pos, 1.0), g_matWorld);
    float4 viewPos = mul(worldPos, g_matView);
    output.pos = mul(viewPos, g_matProjection);

    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWorld).xyz);
    output.uv = worldPos.xyz;
    output.viewDir = normalize(g_matViewInv[3].xyz - worldPos.xyz);
    output.texCoord = input.uv;

    return output;
}

// �ȼ� ���̴�: ���� �⺻ ����(�Ķ���)�� �����ϰ� �ݻ�, ����, ��ָ� ȿ���� �߰�
float4 PS_Main(VS_OUT input) : SV_TARGET
{
    // �ؽ�ó ���ø��� ���� �⺻ ����
    float3 normal = normalize(input.viewNormal);
    float3 viewDir = normalize(input.viewDir);
    float3 baseColor = float3(0.0f, 0.1f, 0.2f);
    
    // **0. ���� �帧
    float time = g_float_0; // �ð� ��
    float2 scrollSpeed = float2(0.1, 0.05); // �� �帧�� �ӵ� (x, y ����)

    float2 scrolledTexCoord = input.texCoord + scrollSpeed * time; // UV ��ǥ �̵�

    
    // **1. ��� �� ����**
    float3 normalMapSample = g_textures[0].Sample(g_sam_0, scrolledTexCoord).rgb; // ��� �� ���ø�
    normalMapSample = normalMapSample * 2.0 - 1.0; // [0, 1] ������ [-1, 1]�� ��ȯ
    
    // **2. ť�� �� �ݻ� ����**
    float3 reflectedDir = reflect(viewDir, normalMapSample);
    float3 cubeReflection = g_texCube.Sample(g_sam_0, float3(-reflectedDir.x, reflectedDir.y, reflectedDir.z)).rgb;

    // **3. �ǽð� �ݻ�(Real-Time Reflection) ����**
    float3 reflectionColor = g_textures[1].Sample(g_sam_0, input.texCoord).rgb;

    // **4. ���� ���**
    float3 refractedDir = refract(viewDir, normal, 1.0 / 1.33); // �� ������ 1.33 ����
    float3 refractionColor = g_refractionTex.Sample(g_sam_0, input.texCoord).rgb;

    // **5. Fresnel ȿ��**
    float fresnelFactor = pow(1.0 - saturate(dot(viewDir, normal)), 3.0);

    // **6. ���� �ݻ� ���� �ռ�**
    float3 finalReflection = lerp(cubeReflection, reflectionColor, fresnelFactor); // Fresnel ������ ���� �ݻ� ����

    // �������� ���� �ռ�
    //float3 finalColor = lerp(refractionColor, finalReflection, fresnelFactor);
    float3 finalColor = lerp(cubeReflection, baseColor, 0.5f);

    return float4(finalReflection, 0.5f);
}

#endif