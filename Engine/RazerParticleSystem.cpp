#include "pch.h"
#include "RazerParticleSystem.h"

RazerParticleSystem::RazerParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.005f, 0.0f);
	SetParticleLifeTime(0.5f, 1.0f);
	SetParticleSpeed(2000.0f, 2500.0f);
	SetParticleScale(200.0f, 200.0f);
	SetDuration(3.0f);
	SetOnceParticleNum(10);
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"RazerParticle")->Clone(), GET_SINGLE(Resources)->Get<Material>(L"RazerComputeParticle")->Clone());
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"RazerParticleTexture", L"..\\Resources\\Texture\\Particle\\razer.png");
	SetParticleTexture(0, texture);
}

void RazerParticleSystem::FinalUpdate()
{
	Vec3 look = GetGameObject()->GetTransform()->GetParent().lock()->GetLook();
	look.Normalize();
	SetEmitDirection(Vec4(look.x, 0.0f, look.z, 0.0f));
	
	BaseParticleSystem::FinalUpdate();
}