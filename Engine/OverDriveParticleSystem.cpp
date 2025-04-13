#include "pch.h"
#include "OverDriveParticleSystem.h"

OverDriveParticleSystem::OverDriveParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.001f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(1000.0f, 800.0f);
	SetParticleScale(200.0f, 100.0f);
	SetDuration(1.0f);
	SetOnceParticleNum(1);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"OverDriveParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"OverDriveComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"OverDriveParticleTexture", L"..\\Resources\\Texture\\Particle\\overDrive.png");
	SetParticleTexture(0, texture);
}

void OverDriveParticleSystem::FinalUpdate()
{
	shared_ptr<Texture> textur2 = GET_SINGLE(Resources)->GetColorTexture();
	SetParticleTexture(1, textur2);

	BaseParticleSystem::FinalUpdate();
}