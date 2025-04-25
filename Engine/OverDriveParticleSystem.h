#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class OverDriveParticleSystem : public BaseParticleSystem
{
public:
	OverDriveParticleSystem();

	virtual void FinalUpdate() override;
};
