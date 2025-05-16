#include "ShaderParams.hlsl"
#include "utils.hlsl"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

struct PS_OUT
{
    float4 diffuse : SV_Target0;
    float4 specular : SV_Target1;
};

VS_OUT VS_DirLight(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}

PS_OUT PS_DirLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;

    float3 viewPos = g_textures.Sample(g_sam_0, input.uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);

    float3 viewNormal = g_textures1.Sample(g_sam_0, input.uv).xyz;
    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);

    // 그림자 계산
    if (length(color.diffuse) != 0)
    {
        
        matrix shadowCameraVP = g_mat_0;

        // 뷰 공간 → 월드 공간
        float4 worldPos = mul(float4(viewPos, 1.f), g_matViewInv);

        // 월드 → 그림자 카메라 공간
        float4 shadowClipPos = mul(worldPos, shadowCameraVP);
        float depth = shadowClipPos.z / shadowClipPos.w;

        // Shadow Map UV 계산
        float2 uv = shadowClipPos.xy / shadowClipPos.w;
        uv.y = -uv.y;
        uv = uv * 0.5f + 0.5f;
        
        depth = depth * 0.5f + 0.5f;
        // UV가 ShadowMap 내부에 있을 때만 그림자 샘플링
        if (uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1)
        {
            
            //pcf 처리
            float bias = max(0.005f * (1.0 - dot(viewNormal, float3(0, 0, -1))), 0.0005f);
            float shadow = 0.0f;

            float2 texelSize = 1.0f / float2(4096, 4096); // ShadowMap 해상도에 맞게 조절

            [unroll]
            for (int y = -1; y <= 1; ++y)
            {
                [unroll]
                for (int x = -1; x <= 1; ++x)
                {
                    float2 offset = float2(x, y) * texelSize;
                    float sampleDepth = g_textures2.Sample(g_sam_0, uv + offset).r;
                    if (depth < sampleDepth + bias)
                    {
                        shadow -= 0.2f;
                    }
                }
            }

            shadow /= 9.0f;
            color.diffuse *= shadow;
            color.specular *= shadow;
        }
    }

    
    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;

    return output;
}

VS_OUT VS_PointLight(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

PS_OUT PS_PointLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    // input.pos = SV_Position = Screen 좌표
    float2 uv = float2(input.pos.x / g_vec2_0.x, input.pos.y / g_vec2_0.y);
    float3 viewPos = g_textures.Sample(g_sam_0, uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);

    int lightIndex = g_int_0;
    float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
    float distance = length(viewPos - viewLightPos);
    if (distance > g_light[lightIndex].range)
        clip(-1);

    float3 viewNormal = g_textures1.Sample(g_sam_0, uv).xyz;

    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);

    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;

    return output;
}

VS_OUT VS_SpotLight(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // SpotLight는 화면 전체를 덮는 Quad 메시를 사용
    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}

PS_OUT PS_SpotLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;

    // 현재 픽셀의 뷰 공간 위치 가져오기
    float3 viewPos = g_textures.Sample(g_sam_0, input.uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1); // 깊이 값이 0 이하라면 픽셀 무시

    // 현재 픽셀의 법선 벡터 가져오기
    float3 viewNormal = normalize(g_textures1.Sample(g_sam_0, input.uv).xyz);

    // SpotLight 정보 가져오기
    int lightIndex = g_int_0;
    LightInfo light = g_light[lightIndex];

    // SpotLight의 뷰 공간 좌표 및 방향 계산
    float3 viewLightPos = mul(float4(light.position.xyz, 1.f), g_matView).xyz;
    float3 viewLightDir = normalize(mul(float4(light.direction.xyz, 0.f), g_matView).xyz);

    // 픽셀에서 SpotLight까지의 방향 벡터
    float3 toLight = normalize(viewLightPos - viewPos);
    float distance = length(viewPos - viewLightPos);

    // SpotLight의 범위를 초과하면 조명 영향 없음
    if (distance > light.range)
        clip(-1);

    // SpotLight의 조명 각도 제한 적용
    float spotEffect = dot(-toLight, viewLightDir);
    float cosInner = cos(radians(light.angle * 0.5f)); // 내부 각도
    float cosOuter = cos(radians(light.angle)); // 외부 각도

    // 조명 감쇠 (부드러운 경계)
    float attenuation = smoothstep(cosOuter, cosInner, spotEffect);
    if (attenuation <= 0)
        clip(-1); // 조명 각도를 벗어나면 픽셀 무시

    // 거리 기반 감쇠 (Inverse Square Law)
    float distanceAttenuation = 1.0f / (1.0f + 0.1f * distance + 0.02f * distance * distance);
    attenuation *= distanceAttenuation;

    // 람버트 조명 모델 적용
    float diffuseFactor = max(dot(viewNormal, toLight), 0.f);
    float4 diffuse = light.color.diffuse * diffuseFactor * attenuation;

    // 스펙큘러 조명 (Blinn-Phong 모델)
    float3 viewCamPos = float3(0, 0, 0); // 뷰 공간에서 카메라는 원점
    float3 viewReflect = reflect(-toLight, viewNormal);
    float3 viewToCam = normalize(viewCamPos - viewPos);
    float specularFactor = pow(max(dot(viewReflect, viewToCam), 0.f), 32.0f);
    float4 specular = light.color.specular * specularFactor * attenuation;
   
    // 최종 출력값 설정
    output.diffuse = diffuse + light.color.ambient;
    output.specular = specular;

    return output;
}

VS_OUT VS_Final(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
    
}

float4 PS_Final(VS_OUT input) : SV_Target
{
    float4 output = (float4)0;

    float4 lightPower = g_textures1.Sample(g_sam_0, input.uv);
    if (lightPower.x == 0.f && lightPower.y == 0.f && lightPower.z == 0.f)
        clip(-1);

    float4 color = g_textures.Sample(g_sam_0, input.uv);
    float4 specular = g_textures2.Sample(g_sam_0, input.uv);

    output = (color * lightPower) + specular;
    return output;
}