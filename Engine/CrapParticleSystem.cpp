#include "pch.h"
#include "CrapParticleSystem.h"

CrapParticleSystem::CrapParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.01f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(2000.0f, 2000.0f);
	SetParticleScale(10.0f, 10.0f);
	SetDuration(1.0f);
	SetOnceParticleNum(5);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"CrapParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"CrapComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"CrapParticleTexture", L"..\\Resources\\Texture\\Particle\\crapParticle.png");
	SetParticleTexture(0, texture);
}

void CrapParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}