#include "pch.h"
#include "DustParticleSystem.h"

DustParticleSystem::DustParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.7f, 0.0f);
	SetParticleLifeTime(10000.0f, 10000.0f);
	SetParticleSpeed(50.0f, 20.0f);
	SetParticleScale(20.0f, 10.0f);
	SetDuration(999999999999.0f);
	SetOnceParticleNum(1);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"DustParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"DustComputeParticle")->Clone());

	shared_ptr<Texture> dust = GET_SINGLE(Resources)->Load<Texture>(L"Dust", L"..\\Resources\\Texture\\Particle\\razer.png");
	shared_ptr<Texture> dust2 = GET_SINGLE(Resources)->Load<Texture>(L"Dust2", L"..\\Resources\\Texture\\LightPillar.png");

	SetParticleTexture(0, dust);
	SetParticleTexture(1, dust2);

	ParticleStart();
}

void DustParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}