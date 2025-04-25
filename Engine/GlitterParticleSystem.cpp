#include "pch.h"
#include "GlitterParticleSystem.h"

GlitterParticleSystem::GlitterParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.05f, 0.0f);
	SetParticleLifeTime(2.0f, 4.0f);
	SetParticleSpeed(3.0f, 6.0f);
	SetParticleScale(1500.0f, 2000.0f);
	SetDuration(5.0f);
	SetOnceParticleNum(1);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"GlitterParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"GlitterComputeParticle")->Clone());
	shared_ptr<Texture> sparkle = GET_SINGLE(Resources)->Load<Texture>(L"Sparkle", L"..\\Resources\\Texture\\Particle\\sparkle.png");

	SetParticleTexture(0, sparkle);
}

void GlitterParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}