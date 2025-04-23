#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class GunFlameParticleSystem : public BaseParticleSystem
{
public:
	GunFlameParticleSystem();

	virtual void FinalUpdate() override;
};

