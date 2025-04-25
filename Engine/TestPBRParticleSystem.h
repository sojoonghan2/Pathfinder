#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class TestPBRParticleSystem : public BaseParticleSystem
{
public:
	TestPBRParticleSystem();

	virtual void FinalUpdate() override;

private:
	float		_floatTime = 0.f;
	float		_baseY = 0.f;
};
