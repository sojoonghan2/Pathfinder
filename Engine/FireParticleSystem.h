#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class FireParticleSystem : public BaseParticleSystem
{
public:
	FireParticleSystem(bool reflection);

	virtual void FinalUpdate() override;
};

