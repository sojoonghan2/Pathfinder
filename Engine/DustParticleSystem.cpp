#include "pch.h"
#include "DustParticleSystem.h"

DustParticleSystem::DustParticleSystem() : BaseParticleSystem()
{
    SetParticleInterval(0.5f, 0.0f);
    SetParticleLifeTime(10000.0f, 10000.0f);
    SetParticleSpeed(100.0f, 50.0f);
    SetParticleScale(30.0f, 20.0f);
    SetDuration(999999999999.0f);
    SetOnceParticleNum(1);
    SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"DustParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"DustComputeParticle")->Clone());

    shared_ptr<Texture> dust = GET_SINGLE(Resources)->Load<Texture>(L"Dust", L"..\\Resources\\Texture\\Particle\\razer.png");

    SetParticleTexture(0, dust);
}


void DustParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}