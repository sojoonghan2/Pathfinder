#include "pch.h"
#include "CrabParticleSystem.h"

CrabParticleSystem::CrabParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.0005f, 0.0f);
	SetParticleLifeTime(0.2f, 0.2f);
	SetParticleSpeed(700.0f, 700.0f);
	SetParticleScale(30.0f, 30.0f);
	SetDuration(1.f);
	SetOnceParticleNum(20);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"CrabParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"CrabComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"CrabParticleTexture", L"..\\Resources\\Texture\\Particle\\crapParticle.png");
	SetParticleTexture(0, texture);
}

void CrabParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}