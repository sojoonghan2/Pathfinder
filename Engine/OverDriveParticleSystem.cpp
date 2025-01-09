#include "pch.h"
#include "OverDriveParticleSystem.h"

OverDriveParticleSystem::OverDriveParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.001f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(300.0f, 200.0f);
	SetParticleScale(50.0f, 30.0f);
	SetDuration(1.0f);
	SetOnceParticleNum(1);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"OverDriveParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"OverDriveComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"OverDriveParticleTexture", L"..\\Resources\\Texture\\Particle\\razer.png");
	SetParticleTexture(0, texture);
}

void OverDriveParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}