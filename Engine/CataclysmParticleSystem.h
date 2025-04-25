#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class CataclysmParticleSystem : public BaseParticleSystem
{
public:
	CataclysmParticleSystem();

	virtual void FinalUpdate() override;
};
