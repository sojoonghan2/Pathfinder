#ifndef TERRAINCUBE_FX_
#define TERRAINCUBE_FX_

#include "params.fx"
#include "utils.fx"

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
    float3 worldNormal : TEXCOORD1;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    if (g_int_1 == 1)
        Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);
    
    // 인스턴스 또는 전역 행렬 사용
    matrix matWorld = (g_int_0 == 1) ? input.matWorld : g_matWorld;
    matrix matWV = (g_int_0 == 1) ? input.matWV : g_matWV;
    matrix matWVP = (g_int_0 == 1) ? input.matWVP : g_matWVP;
    
    output.pos = mul(float4(input.pos, 1.f), matWVP);
    output.uv = input.uv;
    output.viewPos = mul(float4(input.pos, 1.f), matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), matWV).xyz);
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    
    // 월드 공간의 법선 계산 추가
    output.worldNormal = normalize(mul(float4(input.normal, 0.f), matWorld).xyz);
    
    return output;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

float GetBlendFactor(float3 normal, float3 dirVector, float threshold)
{
    float alignment = dot(normal, dirVector);
    return saturate((alignment - threshold) / (1.0f - threshold));
}

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;
    
    // 방향 벡터 정의
    const float3 UP_VECTOR = float3(0, 1, 0);
    const float3 DOWN_VECTOR = float3(0, -1, 0);
    
    // 텍스처 블렌딩을 위한 가중치 계산
    float upFactor = GetBlendFactor(input.worldNormal, UP_VECTOR, 0.7f); // 위쪽 면
    float downFactor = GetBlendFactor(input.worldNormal, DOWN_VECTOR, 0.7f); // 아래쪽 면
    
    // 텍스처 샘플링
    float4 mainTexture = g_textures.Sample(g_sam_0, input.uv);
    float4 bottomTexture = g_textures1.Sample(g_sam_0, input.uv);
    float4 topTexture = g_textures.Sample(g_sam_0, input.uv);
    
    // 최종 색상 블렌딩
    float4 color = mainTexture;
    color = lerp(color, bottomTexture, downFactor);
    color = lerp(color, topTexture, upFactor);
    
    output.position = float4(input.viewPos.xyz, 0.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.color = color;
    
    return output;
}

#endif