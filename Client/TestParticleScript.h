#pragma once
#include "MonoBehaviour.h"

class TestParticleScript : public MonoBehaviour
{
public:
	TestParticleScript();
	virtual ~TestParticleScript();

	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();

private:
};
