#include "pch.h"
#include "FireParticleSystem.h"

FireParticleSystem::FireParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.01f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
<<<<<<< HEAD
	SetParticleSpeed(300.0f, 200.0f);
	SetParticleScale(50.0f, 30.0f);
	SetDuration(1.0f);
	SetOnceParticleNum(100);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"CataclysmParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"CataclysmComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"CataclysmParticleTexture", L"..\\Resources\\Texture\\Particle\\ston.png");
=======
	SetParticleSpeed(50.0f, 30.0f);
	SetParticleScale(10.0f, 50.0f);
	SetDuration(2.0f);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"FireParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"FireComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"FireParticleTexture", L"..\\Resources\\Texture\\Particle\\fire.png");
>>>>>>> parent of cbe179b (대격변 파티클 추가)
	SetParticleTexture(texture);
}

void FireParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}