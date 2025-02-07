#pragma once
#include "MonoBehaviour.h"

class MasterScript : public MonoBehaviour
{
public:
	MasterScript();
	virtual ~MasterScript();

	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();
};

