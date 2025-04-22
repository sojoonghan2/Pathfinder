#include "pch.h"
#include "GunFlameParticleSystem.h"

GunFlameParticleSystem::GunFlameParticleSystem() : BaseParticleSystem()
{
    // 더 짧은 간격으로 파티클 생성 (더 폭발적인 효과)
    SetParticleInterval(0.005f, 0.0f);

    // 더 짧은 수명으로 빠르게 사라지는 효과
    SetParticleLifeTime(0.1f, 0.2f);

    // 빠른 속도로 총구에서 방출
    SetParticleSpeed(5000.0f, 7000.0f);

    // 작은 크기에서 시작해서 빠르게 커지는 효과
    SetParticleScale(5.0f, 30.0f);

    // 발사 효과는 순간적이므로 지속 시간 짧게 설정
    SetDuration(0.15f);

    // 한 번에 더 많은 파티클 생성
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
    BaseParticleSystem::FinalUpdate();
}