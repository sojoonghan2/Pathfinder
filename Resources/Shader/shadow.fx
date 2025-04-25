#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "params.fx"
#include "utils.fx"

// ************************
// 그림자 셰이더
// "그림자 맵"을 생성하는 데 사용
// 깊이 정보만 기록하는 간단한 역할 수행
// ************************

struct VS_IN
{
    float3 pos : POSITION;
    float4 weight : WEIGHT; // 애니메이션용 가중치
    float4 indices : INDICES; // 애니메이션용 본 인덱스
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    float3 finalPos = input.pos;

    // g_int_1 == 1이면 스키닝 수행 (애니메이션용)
    if (g_int_1 == 1)
    {
        float3 dummyNormal = float3(0, 0, 0);
        float3 dummyTangent = float3(0, 0, 0);
        Skinning(finalPos, dummyNormal, dummyTangent, input.weight, input.indices);
    }

    output.pos = mul(float4(finalPos, 1.f), g_matWVP);
    output.clipPos = output.pos;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // depth만 저장 (0~1 range)
    return float4(input.clipPos.z / input.clipPos.w, 0.f, 0.f, 0.f);
}

#endif
