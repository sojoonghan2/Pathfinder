#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class LightPillarParticleSystem : public BaseParticleSystem
{
public:
	LightPillarParticleSystem();

	virtual void FinalUpdate() override;
};

