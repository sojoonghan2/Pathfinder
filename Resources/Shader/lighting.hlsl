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

    // �׸��� ���
    if (length(color.diffuse) != 0)
    {
        
        matrix shadowCameraVP = g_mat_0;

        // �� ���� �� ���� ����
        float4 worldPos = mul(float4(viewPos, 1.f), g_matViewInv);

        // ���� �� �׸��� ī�޶� ����
        float4 shadowClipPos = mul(worldPos, shadowCameraVP);
        float depth = shadowClipPos.z / shadowClipPos.w;

        // Shadow Map UV ���
        float2 uv = shadowClipPos.xy / shadowClipPos.w;
        uv.y = -uv.y;
        uv = uv * 0.5f + 0.5f;
        
        depth = depth * 0.5f + 0.5f;
        // UV�� ShadowMap ���ο� ���� ���� �׸��� ���ø�
        if (uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1)
        {
            
            //pcf ó��
            float bias = max(0.005f * (1.0 - dot(viewNormal, float3(0, 0, -1))), 0.0005f);
            float shadow = 0.0f;

            float2 texelSize = 1.0f / float2(4096, 4096); // ShadowMap �ػ󵵿� �°� ����

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

    // input.pos = SV_Position = Screen ��ǥ
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

    // SpotLight�� ȭ�� ��ü�� ���� Quad �޽ø� ���
    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}

PS_OUT PS_SpotLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;

    // ���� �ȼ��� �� ���� ��ġ ��������
    float3 viewPos = g_textures.Sample(g_sam_0, input.uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1); // ���� ���� 0 ���϶�� �ȼ� ����

    // ���� �ȼ��� ���� ���� ��������
    float3 viewNormal = normalize(g_textures1.Sample(g_sam_0, input.uv).xyz);

    // SpotLight ���� ��������
    int lightIndex = g_int_0;
    LightInfo light = g_light[lightIndex];

    // SpotLight�� �� ���� ��ǥ �� ���� ���
    float3 viewLightPos = mul(float4(light.position.xyz, 1.f), g_matView).xyz;
    float3 viewLightDir = normalize(mul(float4(light.direction.xyz, 0.f), g_matView).xyz);

    // �ȼ����� SpotLight������ ���� ����
    float3 toLight = normalize(viewLightPos - viewPos);
    float distance = length(viewPos - viewLightPos);

    // SpotLight�� ������ �ʰ��ϸ� ���� ���� ����
    if (distance > light.range)
        clip(-1);

    // SpotLight�� ���� ���� ���� ����
    float spotEffect = dot(-toLight, viewLightDir);
    float cosInner = cos(radians(light.angle * 0.5f)); // ���� ����
    float cosOuter = cos(radians(light.angle)); // �ܺ� ����

    // ���� ���� (�ε巯�� ���)
    float attenuation = smoothstep(cosOuter, cosInner, spotEffect);
    if (attenuation <= 0)
        clip(-1); // ���� ������ ����� �ȼ� ����

    // �Ÿ� ��� ���� (Inverse Square Law)
    float distanceAttenuation = 1.0f / (1.0f + 0.1f * distance + 0.02f * distance * distance);
    attenuation *= distanceAttenuation;

    // ����Ʈ ���� �� ����
    float diffuseFactor = max(dot(viewNormal, toLight), 0.f);
    float4 diffuse = light.color.diffuse * diffuseFactor * attenuation;

    // ����ŧ�� ���� (Blinn-Phong ��)
    float3 viewCamPos = float3(0, 0, 0); // �� �������� ī�޶�� ����
    float3 viewReflect = reflect(-toLight, viewNormal);
    float3 viewToCam = normalize(viewCamPos - viewPos);
    float specularFactor = pow(max(dot(viewReflect, viewToCam), 0.f), 32.0f);
    float4 specular = light.color.specular * specularFactor * attenuation;
   
    // ���� ��°� ����
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