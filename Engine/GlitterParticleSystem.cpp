#include "pch.h"
#include "GlitterParticleSystem.h"

GlitterParticleSystem::GlitterParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.1f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(0.0f, 0.0f);
	SetParticleScale(1000.0f, 1000.0f);
	SetDuration(1.0f);
	SetOnceParticleNum(1);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"GlitterParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"GlitterComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"GlitterParticleTexture", L"..\\Resources\\Texture\\Particle\\glitter.png");
	SetParticleTexture(0, texture);
	
}

void GlitterParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}