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
    float4 shadowPos : TEXCOORD3;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    if (g_int_1 == 1)
        Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);
    
    matrix matWorld = (g_int_0 == 1) ? input.matWorld : g_matWorld;
    matrix matWV = (g_int_0 == 1) ? input.matWV : g_matWV;
    matrix matWVP = (g_int_0 == 1) ? input.matWVP : g_matWVP;
    
    output.pos = mul(float4(input.pos, 1.f), matWVP);
    output.uv = input.uv;
    output.viewPos = mul(float4(input.pos, 1.f), matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), matWV).xyz);
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    
    // Compute shadow position (no flip here)
    output.shadowPos = mul(float4(input.pos, 1.f), matWorld);
    output.shadowPos = mul(output.shadowPos, g_mat_0);

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
    
    // Transform to normalized shadow UV
    
    float3 shadowCoord = input.shadowPos.xyz / input.shadowPos.w;
    shadowCoord = shadowCoord * 0.5f + 0.5f;
    // Sample shadow

    float rawShadow = 1.0f;
    if (shadowCoord.x >= 0 && shadowCoord.x <= 1 &&
        shadowCoord.y >= 0 && shadowCoord.y <= 1)
    {
        rawShadow = g_shadowTexture.SampleCmpLevelZero(
            g_shadowSampler, shadowCoord.xy, shadowCoord.z - 1.0f);
    }
    
    // 옆면 vertor를 판단해주는게 없어서 텍스쳐가 따로 안들어감 <<< 그래서 이거 수정해야함 그림자 문제도 해결하고
    
    
    // Only apply shadow on downward faces
    const float3 DOWN_VECTOR = float3(0, -1, 0);
    float downFactor = GetBlendFactor(input.worldNormal, DOWN_VECTOR, 0.7f);
    float shadow = lerp(1.0f, rawShadow, downFactor);
    
    // Texture blending
    const float3 UP_VECTOR = float3(0, 1, 0);
    float upFactor = GetBlendFactor(input.worldNormal, UP_VECTOR, 0.7f);
    float4 topTex = g_textures2.Sample(g_sam_0, input.uv);
    float4 bottomTex = g_textures1.Sample(g_sam_0, input.uv);
 
    float4 mainTex = g_textures.Sample(g_sam_0, input.uv);

    float4 color = mainTex;
    color = lerp(color, topTex, upFactor);
    color = lerp(color, bottomTex, downFactor);
    
    output.position = float4(input.viewPos, 0.f);
    output.normal = float4(input.viewNormal, 0.f);
    output.color = color;
    output.color.rgb *= shadow;
    
    return output;
}

#endif
