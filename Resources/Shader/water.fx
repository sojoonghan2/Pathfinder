#ifndef _WATER_FX_
#define _WATER_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewDir : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
};

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

float4 PS_Main(VS_OUT input) : SV_TARGET
{
    float3 normal = normalize(input.viewNormal);
    float3 viewDir = normalize(input.viewDir);
    float3 baseColor = float3(0.0f, 0.1f, 0.2f);

    float time = g_float_0;
    float2 scrollSpeed = float2(0.1, 0.05);

    // ��ũ�Ѹ��� �ؽ�ó ��ǥ
    float2 scrolledTexCoord = input.texCoord + scrollSpeed * time;

    // �ĵ� ȿ�� �߰�
    float wave = sin(input.texCoord.x * 5.0 + g_float_0) * 0.03;
    scrolledTexCoord += wave;

    // �ؽ�ó ���ø�
    float3 normalMapSample = g_textures[1].Sample(g_sam_0, scrolledTexCoord).rgb;
    normalMapSample = normalMapSample * 2.0 - 1.0;

    // Fresnel Factor ���
    float fresnelFactor = pow(1.0 - saturate(dot(viewDir, normal)), 2.0);

    // ���� �ؽ�ó ���ø�
    float3 refractionColor = g_refractionTex.Sample(g_sam_0, input.texCoord).rgb;

    // ���� ����
    float3 enhancedRefraction = lerp(refractionColor, normalMapSample, 0.3);

    // ���� �÷� ���
    float3 finalColor = lerp(enhancedRefraction, baseColor, fresnelFactor);

    return float4(finalColor, 0.5f);
}

#endif