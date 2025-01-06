#include "pch.h"
#include "FireParticleSystem.h"

FireParticleSystem::FireParticleSystem(bool reflection) : BaseParticleSystem(reflection)
{
	SetParticleInfo(0.01f, 0.0f, 3.0f, 5.0f, 100.0f, 50.0f, 5.0f, 10.0f);
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"FireParticleTexture", L"..\\Resources\\Texture\\Particle\\fire.png");
	SetParticleTexture(texture);
	SetDuration(3.0f);
	SetParticleType(ParticleType::FireParticle);
}

void FireParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}