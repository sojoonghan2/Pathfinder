#include "pch.h"
#include "FireParticleSystem.h"

FireParticleSystem::FireParticleSystem() : BaseParticleSystem()
{
	SetParticleInfo(0.01f, 0.0f, 3.0f, 5.0f, 100.0f, 50.0f, 5.0f, 10.0f);
	SetDuration(3.0f);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"FireParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"FireComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"FireParticleTexture", L"..\\Resources\\Texture\\Particle\\fire.png");
	SetParticleTexture(texture);
}

void FireParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}