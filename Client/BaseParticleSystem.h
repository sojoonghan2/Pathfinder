#pragma once
#include "ParticleSystem.h"

// 파티클에서 공통적으로 사용하는 구조를 담기 위해 만든 중간 단계 클래스
// ParticleSystem과 겹치는 부분이 많아 사실상 무의미한 구조
class BaseParticleSystem : public ParticleSystem
{
public:
	BaseParticleSystem() : ParticleSystem() {}
	virtual ~BaseParticleSystem() {}

public:
	virtual void FinalUpdate() override { ParticleSystem::FinalUpdate(); }
};
