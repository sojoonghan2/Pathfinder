#pragma once
#include "MonoBehaviour.h"

class RazerParticleScript : public MonoBehaviour
{
public:
	RazerParticleScript();
	virtual ~RazerParticleScript();

	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();

private:
};

