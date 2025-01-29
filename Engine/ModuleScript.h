#pragma once
#include "MonoBehaviour.h"

class ModuleScript : public MonoBehaviour
{
public:
    virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();

private:
	bool select = false;
};