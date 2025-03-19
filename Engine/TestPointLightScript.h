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
	void Animation();

	void SetPosition(float x, float z);

private:
	float		_speed = PLAYER_SPEED_MPS * METER_TO_CLIENT;
	bool		_isMove = false;
};

