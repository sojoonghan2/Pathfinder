#include "pch.h"
#include "IceParticleSystem.h"

IceParticleSystem::IceParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.005f, 0.0f);
	SetParticleLifeTime(0.5f, 1.0f);
	SetParticleSpeed(500.0f, 200.0f);
	SetParticleScale(500.0f, 1000.0f);
	SetDuration(0.1f);
	SetOnceParticleNum(15);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"IceParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"IceComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"IceParticleTexture", L"..\\Resources\\Texture\\Particle\\ice1.png");
	SetParticleTexture(0, texture);
}

void IceParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}