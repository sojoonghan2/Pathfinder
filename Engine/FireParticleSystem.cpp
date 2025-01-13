#include "pch.h"
#include "FireParticleSystem.h"

FireParticleSystem::FireParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.01f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(50.0f, 30.0f);
	SetParticleScale(10.0f, 50.0f);
<<<<<<< HEAD
	SetDuration(4.0f);
	SetOnceParticleNum(3);
=======
	SetDuration(2.0f);
>>>>>>> parent of cbe179b (대격변 파티클 추가)
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"FireParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"FireComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"FireParticleTexture", L"..\\Resources\\Texture\\Particle\\fire.png");
	SetParticleTexture(texture);
}

void FireParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}