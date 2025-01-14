#ifndef _PARAMS_FX_
#define _PARAMS_FX_

//텍스쳐 배열 크기 정의
#define MAX_TEXTURES 8

struct LightColor
{
    float4      diffuse;
    float4      ambient;
    float4      specular;
};

struct LightInfo
{
    LightColor  color;
    float4	    position;
    float4	    direction; 
    int		    lightType;
    float	    range;
    float	    angle;
    int  	    padding;
};

// 조명 관련 전역 파라미터
cbuffer GLOBAL_PARAMS : register(b0)
{
    int         g_lightCount;
    float3      g_lightPadding;
    // 최대 50개
    LightInfo   g_light[50];
}
// 변환 행렬 파라미터
cbuffer TRANSFORM_PARAMS : register(b1)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProjection;
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
    row_major matrix g_matViewInv;
};

// 머터리얼 다용도 파라미터
cbuffer MATERIAL_PARAMS : register(b2)
{
    int     g_int_0;
    int     g_int_1;
    int     g_int_2;
    int     g_int_3;
    float   g_float_0;
    float   g_float_1;
    float   g_float_2;
    float   g_float_3;
    int     g_tex_on_0;
    int     g_tex_on_1;
    int     g_tex_on_2;
    int     g_tex_on_3;
    float2  g_vec2_0;
    float2  g_vec2_1;
    float2  g_vec2_2;
    float2  g_vec2_3;
    float4  g_vec4_0;
    float4  g_vec4_1;
    float4  g_vec4_2;
    float4  g_vec4_3;
    row_major float4x4 g_mat_0;
    row_major float4x4 g_mat_1;
    row_major float4x4 g_mat_2;
    row_major float4x4 g_mat_3;
};

// Texture2D 텍스처 정의
Texture2D g_textures[MAX_TEXTURES] : register(t0);

// 뎁스 텍스쳐
Texture2D g_depth : register(t1);

// 굴절 텍스쳐
Texture2D g_refractionTex : register(t2);

// 임시용 텍스쳐
Texture2D g_tex_t3 : register(t3);
Texture2D g_tex_t4 : register(t4);

// TextureCube 텍스처 정의 (큐브맵 추가)
TextureCube g_texCube : register(t6);

// 본 애니메이션 행렬 버퍼
StructuredBuffer<Matrix> g_mat_bone : register(t7);

// 샘플러 상태
SamplerState g_sam_0 : register(s0);

#endif