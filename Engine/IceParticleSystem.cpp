#include "pch.h"
#include "IceParticleSystem.h"

IceParticleSystem::IceParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.005f, 0.0f);
	SetParticleLifeTime(1.5f, 3.0f);
	SetParticleSpeed(150.0f, 20.0f);
	SetParticleScale(50.0f, 100.0f);
	SetDuration(3.5f);
	SetOnceParticleNum(15);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"IceParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"IceComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"IceParticleTexture", L"..\\Resources\\Texture\\Particle\\ice1.png");
	SetParticleTexture(0, texture);
}

void IceParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}