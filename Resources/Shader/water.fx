#ifndef _WATER_FX_
#define _WATER_FX_

#include "ShaderParams.fx"
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

	float time = g_float_0;
	float frequency = 10.0f;
	float speed = 1.0f;
	float amplitude = 10.0f;
    
	input.pos.y += sin(input.pos.x * frequency + time * speed) * amplitude;
	input.pos.y += cos(input.pos.z * frequency + time * speed) * amplitude;

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

	float time = g_float_0;
	float2 scrollSpeed = float2(0.01, 0.007);
	float2 scrolledTexCoord = input.texCoord + scrollSpeed * time;

	float wave = sin(input.texCoord.x * 8.0 + time * 2.0) * 0.001;
	scrolledTexCoord += wave;

	// ¹° ÀÚ½Å ¹à±â
	float3 waterTextureSample = g_textures.Sample(g_sam_0, scrolledTexCoord).rgb; // * ¹à±â
	float3 normalMapSample = g_textures1.Sample(g_sam_0, scrolledTexCoord * 2.0).rgb;
	normalMapSample = normalize(normalMapSample * 2.0 - 1.0);

	float fresnelFactor = pow(1.0 - saturate(dot(viewDir, normal)), 4.0);
	float3 lightDir = normalize(float3(0.2, 1.0, -0.3));
	float3 halfVector = normalize(lightDir + viewDir);
	// Specular Á¶¸í
	float specular = pow(saturate(dot(normal, halfVector)), 32.0) * 0.3; // 0.3: ¹Ý»ç±¤

	float3 combinedColor = lerp(normalMapSample, waterTextureSample, 0.5);
	// ¹Ý»ç ¾ïÁ¦
	combinedColor = lerp(combinedColor, 0.3, fresnelFactor); // 0.3: ¹Ý»ç·®
	combinedColor += specular;
	
	combinedColor = lerp(combinedColor, float3(0.1, 0.2, 0.3), 0.2);
	
	// Åæ ¸ÅÇÎ(»ö»ó ¹à±â)
	combinedColor = combinedColor / (combinedColor + 1.0);

	return float4(combinedColor, 0.75);
}

#endif
