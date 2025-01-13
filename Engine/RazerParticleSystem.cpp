#include "pch.h"
#include "RazerParticleSystem.h"

RazerParticleSystem::RazerParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.01f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(1000.0f, 1000.0f);
	SetParticleScale(70.0f, 70.0f);
	SetDuration(3.0f);
	SetOnceParticleNum(5);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"RazerParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"RazerComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"RazerParticleTexture", L"..\\Resources\\Texture\\Particle\\razer.png");
	SetParticleTexture(texture);
}

void RazerParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}