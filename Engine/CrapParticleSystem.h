#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class CrapParticleSystem : public BaseParticleSystem
{
public:
	CrapParticleSystem();

	virtual void FinalUpdate() override;
};

