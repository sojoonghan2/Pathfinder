#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class IceParticleSystem : public BaseParticleSystem
{
public:
	IceParticleSystem(bool reflection);

	virtual void InitializeParticle() override;
	virtual void FinalUpdate() override;
};

