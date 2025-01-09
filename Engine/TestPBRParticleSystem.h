#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class TestPBRParticleSystem : public BaseParticleSystem
{
public:
	TestPBRParticleSystem();

	virtual void FinalUpdate() override;
};

