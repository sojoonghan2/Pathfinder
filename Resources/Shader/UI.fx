#ifndef _DEFAULT_FX_
#define _DEFAULT_FX_

#include "params.fx"
#include "utils.fx"

// ************************
// Forward Rendering
// 각 픽셀에 대해 한 번의 패스로 모든 처리 수행
// 텍스쳐 샘플링, 노말 매핑, 조명 계산을 순차적으로 진행
// 최종 색상이 바로 화면에 출력
// ************************

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

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
	float4 color = float4(1.f, 1.f, 1.f, 1.f);
	if (g_tex_on_0)
		color = g_textures.Sample(g_sam_0, input.uv);

	float alpha = color.a;

	LightColor totalColor = (LightColor) 0.f;
	for (int i = 0; i < g_lightCount; ++i)
	{
		LightColor colorLight = CalculateLightColor(i, input.viewNormal, input.viewPos);
		totalColor.diffuse += colorLight.diffuse;
		totalColor.ambient += colorLight.ambient;
		totalColor.specular += colorLight.specular;
	}

	color.rgb = (totalColor.diffuse.rgb * color.rgb)
              + (totalColor.ambient.rgb * color.rgb)
              + (totalColor.specular.rgb);

    // 알파값 조정 적용
	color.a = g_float_3;

	return color;
}

// [Texture Shader]
// g_textures[0] : Output Texture
// AlphaBlend : true
struct VS_TEX_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEX_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_TEX_OUT VS_Tex(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

float4 PS_Tex(VS_TEX_OUT input) : SV_Target
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    if (g_tex_on_0)
        color = g_textures.Sample(g_sam_0, input.uv);
    
    // 알파 값이 0이면 픽셀을 버림
    //if (color.a == 0.0f)
    //    discard;

    return color;
}

#endif