#include "pch.h"
#include "GunFlameParticleSystem.h"

GunFlameParticleSystem::GunFlameParticleSystem() : BaseParticleSystem()
{
    // �� ª�� �������� ��ƼŬ ���� (�� �������� ȿ��)
    SetParticleInterval(0.005f, 0.0f);

    // �� ª�� �������� ������ ������� ȿ��
    SetParticleLifeTime(0.1f, 0.2f);

    // ���� �ӵ��� �ѱ����� ����
    SetParticleSpeed(5000.0f, 7000.0f);

    // ���� ũ�⿡�� �����ؼ� ������ Ŀ���� ȿ��
    SetParticleScale(5.0f, 30.0f);

    // �߻� ȿ���� �������̹Ƿ� ���� �ð� ª�� ����
    SetDuration(0.15f);

    // �� ���� �� ���� ��ƼŬ ����
    SetOnceParticleNum(20);

    // ���� ��Ƽ���� ���
    SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"GunFlameParticle")->Clone(),
        GET_SINGLE(Resources)->Get<Material>(L"GunFlameComputeParticle")->Clone());

    // �ؽ�ó �ε�
    shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"GunFlameParticleTexture", L"..\\Resources\\Texture\\Particle\\GunFlame.png");
    SetParticleTexture(0, texture);
}

void GunFlameParticleSystem::FinalUpdate()
{
    BaseParticleSystem::FinalUpdate();
}