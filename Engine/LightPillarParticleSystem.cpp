#include "pch.h"
#include "LightPillarParticleSystem.h"

LightPillarParticleSystem::LightPillarParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.9999f, 0.0f);
	SetParticleLifeTime(10000.0f, 10000.0f);
	SetParticleSpeed(0.0f, 0.0f);
	SetParticleScale(5000.0f, 5000.0f);
	SetDuration(99999.0f);
	SetOnceParticleNum(1);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"LightPillarParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"LightPillarComputeParticle")->Clone());
	shared_ptr<Texture> lightPillar = GET_SINGLE(Resources)->Load<Texture>(L"lightPillar", L"..\\Resources\\Texture\\Particle\\lightPillar.png");
	
	SetParticleTexture(0, lightPillar);
}

void LightPillarParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}