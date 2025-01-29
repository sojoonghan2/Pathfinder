#pragma once
#include "MonoBehaviour.h"

enum { UP, DOWN, WAIT };

class ModuleScript : public MonoBehaviour
{
public:
    virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();

private:
	bool alive = false;
	int way = WAIT;
};