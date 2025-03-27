#pragma once
#include "MonoBehaviour.h"

class PlayerScript : public MonoBehaviour
{
public:
	PlayerScript();
	virtual ~PlayerScript();

	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();
	void Animation();

	void SetPosition(float x, float z);


private:
	float			_speed = 1000.f;
	bool			_isMove = false;
};

