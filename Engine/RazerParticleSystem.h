#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class RazerParticleSystem : public BaseParticleSystem
{
public:
	RazerParticleSystem();

	virtual void FinalUpdate() override;
};

