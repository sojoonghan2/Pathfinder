#include "pch.h"
#include "CrabParticleSystem.h"

CrabParticleSystem::CrabParticleSystem() : BaseParticleSystem()
{
    SetParticleInterval(0.0005f, 0.0008f);
    SetParticleLifeTime(0.2f, 0.6f);
    SetParticleSpeed(1800.0f, 2400.0f);
    SetParticleScale(120.0f, 20.0f);
    SetDuration(0.6f);
    SetOnceParticleNum(45);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"CrabParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"CrabComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"CrabParticleTexture", L"..\\Resources\\Texture\\Particle\\crapParticle.png");
	SetParticleTexture(0, texture);
}

void CrabParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}