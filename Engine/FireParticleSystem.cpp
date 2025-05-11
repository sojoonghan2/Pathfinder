#include "pch.h"
#include "FireParticleSystem.h"

FireParticleSystem::FireParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.0005f, 0.0008f);
	SetParticleLifeTime(0.2f, 0.6f);
	SetParticleSpeed(3600.0f, 4800.0f);
	SetParticleScale(120.0f, 20.0f);
	SetDuration(0.2f);
	SetOnceParticleNum(45);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"FireParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"FireComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"FireParticleTexture", L"..\\Resources\\Texture\\Particle\\fire.png");
	SetParticleTexture(0, texture);
}

void FireParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}