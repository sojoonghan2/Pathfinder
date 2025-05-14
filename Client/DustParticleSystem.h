#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class DustParticleSystem : public BaseParticleSystem
{
public:
	DustParticleSystem();

	virtual void FinalUpdate() override;
};
