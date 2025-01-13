#pragma once
#include "ParticleSystem.h"

class BaseParticleSystem : public ParticleSystem
{
public:
    BaseParticleSystem() : ParticleSystem() {}
    virtual ~BaseParticleSystem() {}

public:
    virtual void FinalUpdate() override { ParticleSystem::FinalUpdate(); }
};

