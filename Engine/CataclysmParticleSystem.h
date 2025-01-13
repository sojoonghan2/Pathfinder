#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class FireParticleSystem : public BaseParticleSystem
{
public:
	FireParticleSystem();

	virtual void FinalUpdate() override;
};

