#include "pch.h"
#include "PortalFrameParticleSystem.h"

PortalFrameParticleSystem::PortalFrameParticleSystem()
    : BaseParticleSystem()
{
    SetParticleInterval(0.01f, 0.0f);
    SetParticleLifeTime(3.0f, 3.0f);
    SetParticleSpeed(0.0f, 0.0f);
    SetParticleScale(200.0f, 200.0f);
    SetDuration(10000.0f);
    SetOnceParticleNum(5);
    SetMaterial(
        GET_SINGLE(Resources)->Get<Material>(L"PortalFrameParticle")->Clone(),
        GET_SINGLE(Resources)->Get<Material>(L"PortalFrameComputeParticle")->Clone()
    );
    shared_ptr<Texture> texture =
        GET_SINGLE(Resources)->Load<Texture>(L"PortalFrameParticleTexture", L"..\\Resources\\Texture\\Particle\\razer.png");

    SetParticleTexture(0, texture);
}

void PortalFrameParticleSystem::FinalUpdate()
{
    BaseParticleSystem::FinalUpdate();
}
