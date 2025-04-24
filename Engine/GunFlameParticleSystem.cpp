#include "pch.h"
#include "GunFlameParticleSystem.h"

GunFlameParticleSystem::GunFlameParticleSystem() : BaseParticleSystem()
{
	SetParticleInterval(0.005f, 0.0f);
	SetParticleLifeTime(0.2f, 0.2f);
	SetParticleSpeed(700.0f, 700.0f);
	SetParticleScale(5.0f, 30.0f);
	SetDuration(10000000.f);
	SetOnceParticleNum(20);

	// 기존 머티리얼 사용
	SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"GunFlameParticle")->Clone(),
		GET_SINGLE(Resources)->Get<Material>(L"GunFlameComputeParticle")->Clone());

	// 텍스처 로드
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"GunFlameParticleTexture", L"..\\Resources\\Texture\\Particle\\GunFlame.png");
	SetParticleTexture(0, texture);
}

void GunFlameParticleSystem::FinalUpdate()
{
	Vec3 look = GetGameObject()->GetTransform()->GetParent().lock()->GetLook();
	look.Normalize();
	SetEmitDirection(Vec4(look.x, 0.0f, look.z, 0.0f));

	BaseParticleSystem::FinalUpdate();
}