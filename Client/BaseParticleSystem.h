#pragma once
#include "ParticleSystem.h"

// ��ƼŬ���� ���������� ����ϴ� ������ ��� ���� ���� �߰� �ܰ� Ŭ����
// ParticleSystem�� ��ġ�� �κ��� ���� ��ǻ� ���ǹ��� ����
class BaseParticleSystem : public ParticleSystem
{
public:
	BaseParticleSystem() : ParticleSystem() {}
	virtual ~BaseParticleSystem() {}

public:
	virtual void FinalUpdate() override { ParticleSystem::FinalUpdate(); }
};
