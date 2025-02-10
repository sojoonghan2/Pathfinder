#pragma once
#include "MonoBehaviour.h"

enum { UP, DOWN, WAIT };

class ModuleScript : public MonoBehaviour
{
public:
    virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();

	bool IsUIPicked(int mouseXpos, int mouseYpos);

private:
	bool alive = false;
	int way = WAIT;

	float _speed = 3000.0f;
	float _acceleration = 2000.0f;
	float _minSpeed = 100.0f;
};