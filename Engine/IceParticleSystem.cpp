#include "pch.h"
#include "IceParticleSystem.h"

IceParticleSystem::IceParticleSystem(bool reflection) : BaseParticleSystem(reflection)
{
	InitializeParticle();
}

void IceParticleSystem::InitializeParticle()
{
	SetParticleInfo(0.000001f, 0.0f, 3.0f, 5.0f, 30.0f, 20.0f, 2.0f, 4.0f);
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Ice", L"..\\Resources\\Texture\\Particle\\ice1.png");
	SetParticleTexture(texture);
	SetDuration(5.0f);
}

void IceParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}