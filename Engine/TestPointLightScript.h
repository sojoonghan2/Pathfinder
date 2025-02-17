#pragma once
#include "MonoBehaviour.h"

class TestPointLightScript : public MonoBehaviour
{
public:
	TestPointLightScript();
	virtual ~TestPointLightScript();

	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();

private:
	float		_speed = 2000.f;
};

