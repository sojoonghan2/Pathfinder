#include "pch.h"
#include "IceParticleSystem.h"

IceParticleSystem::IceParticleSystem() : BaseParticleSystem()
{
	SetParticleInfo(0.01f, 0.0f, 3.0f, 5.0f, 100.0f, 50.0f, 3.0f, 5.0f);
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"IceParticleTexture", L"..\\Resources\\Texture\\Particle\\ice1.png");
	SetParticleTexture(texture);
	SetDuration(1.0f);
	SetParticleType(ParticleType::IceParticle);
}

void IceParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}