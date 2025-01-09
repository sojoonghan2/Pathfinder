#include "pch.h"
#include "OverDriveParticleSystem.h"

OverDriveParticleSystem::OverDriveParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.001f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(300.0f, 250.0f);
	SetParticleScale(50.0f, 30.0f);
	SetDuration(2.0f);
	SetOnceParticleNum(1);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"OverDriveParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"OverDriveComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"OverDriveParticleTexture", L"..\\Resources\\Texture\\Particle\\overDrive.png");
	SetParticleTexture(0, texture);
	shared_ptr<Texture> textur2 = GET_SINGLE(Resources)->Load<Texture>(L"OverDriveWhiteParticleTexture", L"..\\Resources\\Texture\\Particle\\razer.png");
	SetParticleTexture(1, textur2);
}

void OverDriveParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}