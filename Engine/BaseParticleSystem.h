#pragma once
#include "ParticleSystem.h"

class BaseParticleSystem : public ParticleSystem
{
public:
    BaseParticleSystem(bool refraction) : ParticleSystem(refraction) {}
    virtual ~BaseParticleSystem() {}

public:
    // 기본 파티클 시스템 초기화
    virtual void InitializeParticle() { SetParticleInfo(0.1f, 0.0f, 1.0f, 2.0f, 10.0f, 20.0f, 1.0f, 2.0f); }

    virtual void FinalUpdate() override { ParticleSystem::FinalUpdate(); }
};

