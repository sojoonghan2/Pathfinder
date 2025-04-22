#include "pch.h"
#include "GunFlameParticleSystem.h"

GunFlameParticleSystem::GunFlameParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.01f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(2000.0f, 2000.0f);
	SetParticleScale(10.0f, 10.0f);
	SetDuration(1.0f);
	SetOnceParticleNum(5);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"GunFlameParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"GunFlameComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"CrapParticleTexture", L"..\\Resources\\Texture\\Particle\\GunFlame.png");
	SetParticleTexture(0, texture);
}

void GunFlameParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}