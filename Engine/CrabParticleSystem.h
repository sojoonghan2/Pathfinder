#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class CrabParticleSystem : public BaseParticleSystem
{
public:
	CrabParticleSystem();

	virtual void FinalUpdate() override;
};

