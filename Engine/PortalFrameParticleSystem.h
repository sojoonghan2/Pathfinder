#pragma once
#include "BaseParticleSystem.h"
#include "Resources.h"

class PortalFrameParticleSystem : public BaseParticleSystem
{
public:
	PortalFrameParticleSystem();

	virtual void FinalUpdate() override;
};

