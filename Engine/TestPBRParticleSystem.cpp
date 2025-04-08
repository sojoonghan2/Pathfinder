#include "pch.h"
#include "TestPBRParticleSystem.h"

TestPBRParticleSystem::TestPBRParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.01f, 0.002f);
	SetParticleLifeTime(10000000.0f, 10000000.0f);
	SetParticleSpeed(0.0f, 0.0f);
	SetParticleScale(3000.f, 3000.f);
	SetDuration(10000000.0f);
	SetOnceParticleNum(6);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"TestPBRParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"TestPBRComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"TestPBRComputeParticle", L"..\\Resources\\Texture\\Particle\\tmp.png");
	SetParticleTexture(0, texture);
}

void TestPBRParticleSystem::FinalUpdate()
{
	shared_ptr<Texture> screenTexture =
		GET_SINGLE(Resources)->GetRenderTargetTexture();
	SetParticleTexture(1, screenTexture);

	shared_ptr<Texture> colorTexture =
		GET_SINGLE(Resources)->GetColorTexture();
	SetParticleTexture(2, colorTexture);

	BaseParticleSystem::FinalUpdate();
}