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

	// 씬 전환될 때 호출
	void ApplyModuleAbillities();
};

