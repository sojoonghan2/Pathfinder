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
	void SelectedAnimation();

private:
	bool alive;
	int way = WAIT;
	bool selected;

	float _speed = 3000.0f;
	float _acceleration = 2000.0f;
	float _minSpeed = 100.0f;
};