#include "pch.h"
#include "CrapParticleSystem.h"

CrapParticleSystem::CrapParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.0005f, 0.0f);
	SetParticleLifeTime(0.2f, 0.2f);
	SetParticleSpeed(700.0f, 700.0f);
	SetParticleScale(30.0f, 30.0f);
	SetDuration(1.f);
	SetOnceParticleNum(20);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"CrapParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"CrapComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"CrapParticleTexture", L"..\\Resources\\Texture\\Particle\\crapParticle.png");
	SetParticleTexture(0, texture);
}

void CrapParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}