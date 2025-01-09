#include "pch.h"
#include "TestPBRParticleSystem.h"

TestPBRParticleSystem::TestPBRParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.001f, 0.0f);
	SetParticleLifeTime(3.0f, 5.0f);
	SetParticleSpeed(0.0f, 0.0f);
	SetParticleScale(100.0f, 100.0f);
	SetDuration(1000.0f);
	SetOnceParticleNum(1);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"TestPBRParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"CataclysmComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"TestPBRComputeParticle", L"..\\Resources\\Texture\\Particle\\tmp.png");
	SetParticleTexture(0, texture);
}

void TestPBRParticleSystem::FinalUpdate()
{
	BaseParticleSystem::FinalUpdate();
}