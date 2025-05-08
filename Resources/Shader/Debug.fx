#ifndef _DEBUG_FX_
#define _DEBUG_FX_

#include "params.fx"
#include "utils.fx"

cbuffer Material : register(b2)
{
	float4 gColor;
};

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
	float3 viewTangent : TANGENT;
	float3 viewBinormal : BINORMAL;
};

VS_OUT VS_Debug(VS_IN input)
{
	VS_OUT output = (VS_OUT) 0;

	output.pos = mul(float4(input.pos, 1.f), g_matWVP);
	output.uv = input.uv;

	output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
	output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
	output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
	output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));

	return output;
}

float4 PS_Debug(VS_OUT input) : SV_Target
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

#endif