#include "pch.h"
#include "CataclysmParticleSystem.h"

CataclysmParticleSystem::CataclysmParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.001f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(300.0f, 200.0f);
	SetParticleScale(5.0f, 5.0f);
	SetDuration(1.0f);
	SetOnceParticleNum(50);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"CataclysmParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"CataclysmComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"CataclysmParticleTexture", L"..\\Resources\\Texture\\Particle\\ston.png");
	SetParticleTexture(texture);
}

void CataclysmParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}