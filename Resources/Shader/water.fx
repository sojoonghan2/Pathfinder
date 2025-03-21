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
    float3 baseColor = float3(0.3f, 0.3f, 1.f);

    float time = g_float_0;
    float2 scrollSpeed = float2(0.01, 0.007);

    // 스크롤링된 텍스처 좌표
    float2 scrolledTexCoord = input.texCoord + scrollSpeed * time;

    // 파동 효과 추가
    float wave = sin(input.texCoord.x * 8.0 + time * 2.0) * 0.001;
    scrolledTexCoord += wave;

    // 물 텍스처 샘플링
    float3 waterTextureSample = g_textures.Sample(g_sam_0, scrolledTexCoord).rgb * 1.2;

    // 노멀 맵 샘플링
    float3 normalMapSample = g_textures1.Sample(g_sam_0, scrolledTexCoord * 2.0).rgb;
    normalMapSample = normalMapSample * 2.0 - 1.0;

    // Fresnel Factor
    float fresnelFactor = pow(1.0 - saturate(dot(viewDir, normal)), 4.0);

    // 굴절 텍스처 샘플링
    float2 refractionTexCoord = input.texCoord + normalMapSample.xy * 0.05;
    float3 refractionColor = g_textures2.Sample(g_sam_0, refractionTexCoord).rgb;

    // 반사 효과 추가
    float3 reflectionColor = g_textures3.Sample(g_sam_0, reflect(-viewDir, normal).xy * 0.5 + 0.5).rgb;
    reflectionColor *= fresnelFactor;

    // Specular 하이라이트 추가
    float3 lightDir = normalize(float3(0.2, 1.0, -0.3));
    float3 halfVector = normalize(lightDir + viewDir);
    float specular = pow(saturate(dot(normal, halfVector)), 32.0) * 0.5;

    // 최종 컬러 결합
    float3 combinedColor = lerp(refractionColor, normalMapSample, 0.5);
    combinedColor = lerp(combinedColor, waterTextureSample, 0.5);
    combinedColor = lerp(combinedColor, reflectionColor, fresnelFactor);
    combinedColor += specular;

    return float4(combinedColor, 0.9f);
}

#endif