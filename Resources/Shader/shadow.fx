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
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0.f;
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.clipPos = output.pos;
   
    return output;
}


float4 PS_Main(VS_OUT input) : SV_Target
{
    return float4(input.clipPos.z / input.clipPos.w, 0.f, 0.f, 0.f);
}


#endif