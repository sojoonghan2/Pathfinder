#ifndef _OCCUPATION_FX_
#define _OCCUPATION_FX_

#include "params.fx"
#include "utils.fx"

// ���۵� ���̴��� ����
struct VS_IN
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 weight : WEIGHT;
	float4 indices : INDICES;

	row_major matrix matWorld : W;
	row_major matrix matWV : WV;
	row_major matrix matWVP : WVP;
	uint instanceID : SV_InstanceID;
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

VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output;

    // ��Ű�� ����
	if (g_int_1 == 1)
		Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);

    // �ν��Ͻ� ���ο� ���� ��Ʈ���� ����
	if (g_int_0 == 1)
	{
		output.pos = mul(float4(input.pos, 1.f), input.matWVP);
		output.viewPos = mul(float4(input.pos, 1.f), input.matWV).xyz;
		output.viewNormal = normalize(mul(float4(input.normal, 0.f), input.matWV).xyz);
		output.viewTangent = normalize(mul(float4(input.tangent, 0.f), input.matWV).xyz);
	}
	else
	{
		output.pos = mul(float4(input.pos, 1.f), g_matWVP);
		output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
		output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
		output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
	}

	output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
	output.uv = input.uv;

	return output;
}

// G-Buffer�� �ȼ� ���̴� ��� ����
struct PS_OUT
{
	float4 position : SV_Target0;
	float4 normal : SV_Target1;
	float4 color : SV_Target2;
};

PS_OUT PS_Main(VS_OUT input)
{
	PS_OUT output;
    // ����Ʈ �÷� �Ǵ� �ؽ�ó
	float4 color = float4(1.f, 1.f, 1.f, 1.f);
	if (g_tex_on_0 == 1)
		color = g_textures.Sample(g_sam_0, input.uv);
    
	float alphaMin = 0.3f;
	float alphaMax = 0.7f;
	float alphaMid = (alphaMin + alphaMax) * 0.5f;
	float alphaHalfRange = (alphaMax - alphaMin) * 0.5f;
	float oscillation = sin(g_float_0 * 2.0f);
	float animatedAlpha = alphaMid + oscillation * alphaHalfRange;
	
	// ���İ� ����
	color.a = color.a * animatedAlpha;
    
    // ���İ� 0�̸� �ȼ� ����
	if (color.a == 0.0f)
		discard;
    // �븻 ����
	float3 viewNormal = input.viewNormal;
	if (g_tex_on_1 == 1)
	{
		float3 tangentSpaceNormal = g_textures1.Sample(g_sam_0, input.uv).xyz;
		tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
		float3x3 matTBN = float3x3(
            input.viewTangent,
            input.viewBinormal,
            input.viewNormal
        );
		viewNormal = normalize(mul(matTBN, tangentSpaceNormal));
	}
	output.position = float4(input.viewPos, 0.f);
	output.normal = float4(viewNormal, 0.f);
	output.color = color;
	return output;
}

#endif