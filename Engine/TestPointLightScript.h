#pragma once
#include "MonoBehaviour.h"

enum class PLAYERWAY_TYPE : uint8
{
	FRONT,
	BACK,
	LEFT,
	RIGHT
};

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
	float			_speed = 1000.f;
	bool			_isMove = false;

	PLAYERWAY_TYPE	_way;
};

