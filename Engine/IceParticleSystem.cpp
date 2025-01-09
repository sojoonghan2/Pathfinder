#include "pch.h"
#include "IceParticleSystem.h"

IceParticleSystem::IceParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.01f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(100.0f, 50.0f);
	SetParticleScale(3.0f, 5.0f);
	SetDuration(1.0f);
	SetOnceParticleNum(5);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"IceParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"IceComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"IceParticleTexture", L"..\\Resources\\Texture\\Particle\\ice1.png");
	SetParticleTexture(0, texture);
}

void IceParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}