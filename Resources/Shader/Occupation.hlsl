#include "ShaderParams.hlsl"
#include "utils.hlsl"

// 포워드 렌더링용 구조
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
    // 스키닝 적용
	if (g_int_1 == 1)
		Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);
    // 인스턴싱 여부에 따라 매트릭스 적용
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

// 포워드 렌더링용 픽셀 셰이더 - 바로 최종 색상 출력
float4 PS_Main(VS_OUT input) : SV_Target
{
    // 디폴트 컬러 또는 텍스처
	float4 color = float4(1.f, 1.f, 1.f, 1.f);
	if (g_tex_on_0 == 1)
		color = g_textures.Sample(g_sam_0, input.uv);
    
	// 알파값 alphaMin ~ alphaMax
	float alphaMin = 0.3f;
	float alphaMax = 0.7f;
	float alphaMid = (alphaMin + alphaMax) * 0.5f;
	float alphaHalfRange = (alphaMax - alphaMin) * 0.5f;
	float oscillation = sin(g_float_0 * 2.0f);
	float animatedAlpha = alphaMid + oscillation * alphaHalfRange;
	
	// 알파값 적용
	color.a = color.a * animatedAlpha;
    
    // 알파가 0이면 픽셀 버림
	if (color.a == 0.0f)
		discard;
	
	return color;
}