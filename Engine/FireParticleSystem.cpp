#include "pch.h"
#include "FireParticleSystem.h"

FireParticleSystem::FireParticleSystem(bool reflection) : BaseParticleSystem(reflection)
{
	InitializeParticle();
}

void FireParticleSystem::InitializeParticle()
{
	SetParticleInfo(0.000001f, 0.0f, 3.0f, 5.0f, 100.0f, 50.0f, 30.0f, 50.0f);
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"FireParticleTexture", L"..\\Resources\\Texture\\Particle\\fire.png");
	SetParticleTexture(texture);
	SetDuration(1.0f);
}

void FireParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}