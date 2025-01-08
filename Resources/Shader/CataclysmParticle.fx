#ifndef _FIREPARTICLE_FX_
#define _FIREPARTICLE_FX_

#include "params.fx"
#include "utils.fx"

#define PI 3.141592

struct Particle
{
    float3  worldPos;
    float   curTime;
    float3  worldDir;
    float   lifeTime;
    int     alive;
    float3  padding;
};

// ±×·¡ÇÈ½º ¼ÎÀÌ´õ
// StructuredBuffer Á¤ÀÇ
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

// VS_MAIN
// g_float_0        : Start Scale
// g_float_1        : End Scale
// g_textures[0]    : Particle Texture

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
    uint id : SV_InstanceID;
};

// ÆÄÆ¼Å¬ÀÇ ÇöÀç ¼ö¸í ºñÀ²¿¡ µû¶ó Å©±â¸¦ °è»êÇÏ°í, È­¸é¿¡ »ç°¢Çü ÇüÅÂ·Î È®Àå
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

    // ÆÄÆ¼Å¬ ¼ö¸í ÁøÇà·ü ºñÀ² °è»ê
    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float scale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;

    // View Space
    output[0].position = vtx.viewPos + float4(-scale, scale, 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(scale, scale, 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(scale, -scale, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-scale, -scale, 0.f, 0.f);

    // Projection Space
    output[0].position = mul(output[0].position, g_matProjection);
    output[1].position = mul(output[1].position, g_matProjection);
    output[2].position = mul(output[2].position, g_matProjection);
    output[3].position = mul(output[3].position, g_matProjection);

    // uv
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    // id
    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;

    // »ï°¢Çü ½ºÆ®¸³ Ãß°¡
    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

float4 PS_Main(GS_OUT input) : SV_Target
{
    // ÆÄÆ¼Å¬ ¼ö¸í ºñÀ² °è»ê
    float ratio = g_data[input.id].curTime / g_data[input.id].lifeTime;

    // ³ë¶õ»ö(ÃÊ±â), »¡°£»ö(Áß°£), °ËÀº»ö(ÃÖÁ¾) »ö»ó Á¤ÀÇ
    float3 startColor = float3(1.0f, 1.0f, 0.0f); // ³ë¶õ»ö
    float3 midColor = float3(1.0f, 0.0f, 0.0f); // »¡°£»ö
    float3 endColor = float3(0.0f, 0.0f, 0.0f); // °ËÀº»ö

<<<<<<< HEAD
    float3 color = lerp(startColor, endColor, ratio);

    // Åõ¸íµµ È¿°ú
    float alpha = 0.8f - ratio; // ¼­¼­È÷ Åõ¸íÇØÁü
=======
    // »ö»ó ÀüÈ¯
    float3 color;
    if (ratio < 0.5f)
    {
        // 0~0.5 ±¸°£: ³ë¶õ»ö -> »¡°£»ö
        color = lerp(startColor, midColor, ratio * 2.0f);
    }
    else
    {
        // 0.5~1.0 ±¸°£: »¡°£»ö -> °ËÀº»ö
        color = lerp(midColor, endColor, (ratio - 0.5f) * 2.0f);
    }
>>>>>>> parent of cbe179b (ëŒ€ê²©ë³€ íŒŒí‹°í´ ì¶”ê°€)

    // ÅØ½ºÃ³ »ùÇÃ¸µ ¹× »ö»ó Àû¿ë
    float4 texColor = g_textures[0].Sample(g_sam_0, input.uv);
    return float4(color, 1.0f) * texColor;
}


struct ComputeShared
{
    int addCount;
    float3 padding;
};

// ÄÄÇ»Æ® ¼ÎÀÌ´õ ÀÔ/Ãâ·Â
// ÆÄÆ¼Å¬ µ¥ÀÌÅÍ°¡ ÀúÀåµÈ ¹öÆÛ
RWStructuredBuffer<Particle> g_particle : register(u0);
// È°¼ºÈ­ÇÒ ÆÄÆ¼Å¬ ¼ö¸¦ °ü¸®ÇÏ´Â °øÀ¯ µ¥ÀÌÅÍ
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

// CS_Main
// g_vec2_1 : µ¨Å¸ ½Ã°£ / ´©Àû ½Ã°£
// g_int_0  : ÃÖ´ë ÆÄÆ¼Å¬ ¼ö
// g_int_1  : »õ·Î È°¼ºÈ­ÇÒ ÆÄÆ¼Å¬ ¼ö
// g_vec4_0 : ÃÖ¼Ò/ÃÖ´ë ¼ö¸í°ú ¼Óµµ
[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.x >= g_int_0)
        return;

    // ÃÊ±â ½ÃÀÛ ¹üÀ§
    float minX = -10.0f, maxX = 10.0f;
    float minZ = -10.0f, maxZ = 10.0f;

    int maxCount = g_int_0;
    int addCount = g_int_1;
    float deltaTime = g_vec2_1.x;
    float accTime = g_vec2_1.y;
    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;
    float minSpeed = g_vec4_0.z;
    float maxSpeed = g_vec4_0.w;

    // È°¼ºÈ­ °¡´ÉÇÑ ÆÄÆ¼Å¬ ¼ö °ü¸®
    g_shared[0].addCount = addCount;
    // ½º·¹µå µ¿±âÈ­
    GroupMemoryBarrierWithGroupSync();

    // ºñÈ°¼º ÆÄÆ¼Å¬ È°¼ºÈ­
    if (g_particle[threadIndex.x].alive == 0 && g_shared[0].addCount > 0)
    {
        // È°¼ºÈ­ °¡´ÉÇÑ ÆÄÆ¼Å¬ ¼ö °¨¼Ò
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

        // ºÒ ÆÄÆ¼Å¬ ¿¬»ê
        if (g_particle[threadIndex.x].alive == 1)
        {
            float x = ((float) threadIndex.x / (float) maxCount) + accTime;
            // »ï°¢Çü ²ÀÁşÁ¡ Á¤ÀÇ
            float2 A = float2(-300.0f, 500.0f);
            float2 B = float2(300.0f, 500.0f);
            float2 C = float2(0.0f, -50.0f);

<<<<<<< HEAD
            // ±ÕÀÏÇÑ »ï°¢Çü ºĞÆ÷¸¦ À§ÇÑ ¼öÁ¤µÈ ¹Ù¸®¼¾Æ®¸¯ ÁÂÇ¥ »ı¼º
            float r1 = Rand(float2(x, accTime));
            float r2 = Rand(float2(x * accTime, accTime));
    
            // ¼öÁ¤µÈ ¹Ù¸®¼¾Æ®¸¯ ÁÂÇ¥ °è»ê
            float u = 1.0f - sqrt(r1);
            float v = r2 * sqrt(r1);
            float w = 1.0f - u - v;
    
            // »ï°¢Çü ³»ºÎÀÇ ·£´ı ÁÂÇ¥ °è»ê
            float2 randPosition = u * A + v * B + w * C;
    
            g_particle[threadIndex.x].worldPos = float3(randPosition.x, 0.0f, randPosition.y);
            
            // À§Ä¡¿Í µ¶¸³ÀûÀÎ ¼ö¸í ¼³Á¤
            float randomSeed = Rand(float2(accTime, threadIndex.x));
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * randomSeed) + minLifeTime;
=======
            float r1 = Rand(float2(x, accTime));
            float r2 = Rand(float2(x * accTime, accTime));
            float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));

            g_particle[threadIndex.x].worldPos = float3(
                lerp(minX, maxX, r1), // X: ¹üÀ§ ³» ·£´ı °ª
                0.0f, // Y: °íÁ¤°ª
                lerp(minZ, maxZ, r3) // Z: ¹üÀ§ ³» ·£´ı °ª
            );

            g_particle[threadIndex.x].worldDir = normalize(float3(r1 - 0.5f, 1.0f, r3 - 0.5f));
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * r2) + minLifeTime;
>>>>>>> parent of cbe179b (ëŒ€ê²©ë³€ íŒŒí‹°í´ ì¶”ê°€)
            g_particle[threadIndex.x].curTime = 0.f;
        }
    }
    // ±âÁ¸ ÆÄÆ¼Å¬ ¾÷µ¥ÀÌÆ®
    else
    {
        // ÇöÀç ½Ã°£ ¾÷µ¥ÀÌÆ®
        g_particle[threadIndex.x].curTime += deltaTime;
        
        // ÁøÇà·ü °è»ê (0~1 »çÀÌ °ª)
        float progress = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
        
        // ZÃà ¼Óµµ °è»ê
        float ySpeed;
        if (progress < 0.03f)
        {
            // Ã³À½¿¡´Â ºü¸£°Ô -z ¹æÇâÀ¸·Î ÀÌµ¿
            ySpeed = 300.0f; // ºü¸¥ ¼Óµµ, °ªÀº Á¶Á¤ °¡´É
        }
        else if (progress > 0.2f)
        {
            // 0.5 ÀÌÈÄ¿¡´Â ÃµÃµÈ÷ +z ¹æÇâÀ¸·Î ÀÌµ¿
            ySpeed = -50.0f; // ´À¸° ¼Óµµ, °ªÀº Á¶Á¤ °¡´É
        }
<<<<<<< HEAD
        
        // À§Ä¡ ¾÷µ¥ÀÌÆ®
        g_particle[threadIndex.x].worldPos.y += ySpeed * deltaTime;
        
        // ¼ö¸íÀÌ ´ÙÇÑ ÆÄÆ¼Å¬ Á¦°Å
        if (g_particle[threadIndex.x].curTime >= g_particle[threadIndex.x].lifeTime)
        {
            g_particle[threadIndex.x].alive = 0;
        }
=======

        float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
        float speed = (maxSpeed - minSpeed) * ratio + minSpeed;
        g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
>>>>>>> parent of cbe179b (ëŒ€ê²©ë³€ íŒŒí‹°í´ ì¶”ê°€)
    }
}

#endif