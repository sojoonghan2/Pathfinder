#ifndef _TESTPBRPARTICLE_FX_
#define _TESTPBRPARTICLE_FX_

#include "params.fx"
#include "utils.fx"

struct Particle
{
    float3 worldPos;
    float curTime;
    float3 worldDir;
    float lifeTime;
    int alive;
    float rotationAngle;
    float2 padding;
};

// StructuredBuffer 정의
StructuredBuffer<Particle> g_data : register(t9);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint id : SV_InstanceID;
};

struct VS_OUT
{
    float4 viewPos : POSITION;
    float2 uv : TEXCOORD;
    float id : ID;
};

// Vertex Shader
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    float3 worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;
    worldPos += g_data[input.id].worldPos;

    output.viewPos = mul(float4(worldPos, 1.f), g_matView);
    output.uv = input.uv;
    output.id = input.id;

    return output;
}

struct GS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
    uint id : SV_InstanceID;
};

// Geometry Shader
[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output[4] =
    {
        (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f
    };

    VS_OUT vtx = input[0];
    uint id = (uint) vtx.id;
    if (0 == g_data[id].alive)
        return;

    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float scale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;
    
    // Get rotation angle from particle data
    float rotAngle = g_data[id].rotationAngle;
    
    // Calculate sine and cosine for rotation
    float sinAngle = sin(rotAngle);
    float cosAngle = cos(rotAngle);
    
    // Base corners in view space (before rotation)
    float4 corners[4] =
    {
        float4(-scale, scale, 0.f, 0.f),
        float4(scale, scale, 0.f, 0.f),
        float4(scale, -scale, 0.f, 0.f),
        float4(-scale, -scale, 0.f, 0.f)
    };
    
    // Rotate each corner
    for (int i = 0; i < 4; i++)
    {
        float x = corners[i].x;
        float y = corners[i].y;
        corners[i].x = x * cosAngle - y * sinAngle;
        corners[i].y = x * sinAngle + y * cosAngle;
    }

    // View Space with rotation applied
    output[0].position = vtx.viewPos + corners[0];
    output[1].position = vtx.viewPos + corners[1];
    output[2].position = vtx.viewPos + corners[2];
    output[3].position = vtx.viewPos + corners[3];

    // Projection Space
    output[0].position = mul(output[0].position, g_matProjection);
    output[1].position = mul(output[1].position, g_matProjection);
    output[2].position = mul(output[2].position, g_matProjection);
    output[3].position = mul(output[3].position, g_matProjection);

    // UV
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    // Portal edge glow color - orange like Doctor Strange's portal
    float4 portalColor = float4(1.0f, 0.6f, 0.0f, ratio * (1.0f - ratio) * 4.0f); // Brighter at middle of lifetime
    
    // Apply color to all vertices
    output[0].color = portalColor;
    output[1].color = portalColor;
    output[2].color = portalColor;
    output[3].color = portalColor;

    // ID
    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

// Pixel Shader
float4 PS_Main(GS_OUT input) : SV_Target
{
    // 파티클 텍스처 샘플링
    float4 particleColor = g_textures.Sample(g_sam_0, input.uv);
    
    // Add portal edge glow effect
    float distFromCenter = length(input.uv - float2(0.5f, 0.5f)) * 2.0f;
    float edgeGlow = 1.0f - smoothstep(0.8f, 1.0f, distFromCenter);
    float innerPortal = step(0.85f, distFromCenter); // Inner transparent area
    
    // Combine with input color from geometry shader
    float4 portalEdgeColor = input.color * edgeGlow;
    
    // Combine colors
    float4 finalColor = particleColor * portalEdgeColor;
    
    // Control the inner transparent area of the portal
    if (distFromCenter < 0.85f)
    {
        // Inner area - apply refraction effect
        
        // Refracted UV calculation
        float2 refractedUV = input.uv + (particleColor.rg - 0.5f) * 0.05f;
        refractedUV = saturate(refractedUV);
        
        // Sample background through portal
        float4 backgroundColor = g_textures2.Sample(g_sam_0, refractedUV);
        
        // Mix with a slight tint to give portal color
        return lerp(backgroundColor, float4(0.0f, 0.7f, 1.0f, 0.3f), 0.2f);
    }
    else
    {
        // Edge area - glowing effect
        return finalColor;
    }
}

// Compute Shader
struct ComputeShared
{
    int addCount;
    float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

groupshared int isGenerated;

[numthreads(1, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.x >= g_int_0)
        return;

    int maxCount = g_int_0;
    int addCount = g_int_1;
    int frameNumber = g_int_2;
    float deltaTime = g_vec2_1.x;
    float accTime = g_vec2_1.y;
    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;
    float minSpeed = g_vec4_0.z;
    float maxSpeed = g_vec4_0.w;

    g_shared[0].addCount = addCount;
    GroupMemoryBarrierWithGroupSync();

    if (g_particle[threadIndex.x].alive == 0)
    {
        while (true)
        {
            int remaining = g_shared[0].addCount;
            if (remaining <= 0)
                break;

            int expected = remaining;
            int desired = remaining - 1;
            int originalValue;
            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);

            if (originalValue == expected)
            {
                g_particle[threadIndex.x].alive = 1;
                break;
            }
        }

        if (g_particle[threadIndex.x].alive == 1)
        {
            float x = ((float) threadIndex.x / (float) maxCount) + accTime;

            float r1 = Rand(float2(x, accTime));
            float r2 = Rand(float2(x * accTime, accTime));
            float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));

            // Calculate position on a circle
            float angle = 2.0f * 3.14159f * r1; // Random angle around circle
            float radius = 5.0f; // Portal radius
            
            // Position particles in a ring formation
            float3 position;
            position.x = radius * cos(angle);
            position.y = radius * sin(angle);
            position.z = 0.0f; // All in same plane for portal effect
            
            g_particle[threadIndex.x].worldPos = position;
            g_particle[threadIndex.x].worldDir = normalize(float3(cos(angle), sin(angle), 0.0f));
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * r2) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f;
            
            // Set initial rotation angle based on position in circle
            g_particle[threadIndex.x].rotationAngle = angle;
        }
    }
    else
    {
        // Update existing particles
        g_particle[threadIndex.x].curTime += deltaTime;
        
        if (g_particle[threadIndex.x].curTime >= g_particle[threadIndex.x].lifeTime)
        {
            g_particle[threadIndex.x].alive = 0;
        }
        else
        {
            // Rotate particles over time to create spinning portal effect
            g_particle[threadIndex.x].rotationAngle += deltaTime * 2.0f;
            
            // Calculate current progress ratio of lifetime
            float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
            
            // Portal opening animation: radius increases over time then stabilizes
            float initialRadius = 2.0f;
            float finalRadius = 5.0f;
            float currentRadius;
            
            if (ratio < 0.3f)
            {
                // Opening phase
                currentRadius = lerp(initialRadius, finalRadius, ratio / 0.3f);
            }
            else
            {
                // Stable phase
                currentRadius = finalRadius;
            }
            
            // Keep particles in ring formation with updated radius
            float angle = g_particle[threadIndex.x].rotationAngle;
            g_particle[threadIndex.x].worldPos.x = currentRadius * cos(angle);
            g_particle[threadIndex.x].worldPos.y = currentRadius * sin(angle);
        }
    }
}

#endif