#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class IceParticleSystem : public BaseParticleSystem
{
public:
	IceParticleSystem();

	virtual void FinalUpdate() override;
};

