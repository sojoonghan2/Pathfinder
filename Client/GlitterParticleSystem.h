#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class GlitterParticleSystem : public BaseParticleSystem
{
public:
	GlitterParticleSystem();

	virtual void FinalUpdate() override;
};
