#pragma once
#include "MonoBehaviour.h"

class TestCameraScript : public MonoBehaviour
{
public:
	TestCameraScript();
	virtual ~TestCameraScript();

	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();

private:
	float		_speed = 10000.f;
};

