#pragma once
#include "MonoBehaviour.h"

enum class StageState
{
	START,
	PLAYING,
	PORTAL_CREATE,
	PORTAL_ENTERED,
	MODULE_SELECTI,
	LOAD_NEXT_SCENE
};

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

protected:
	StageState _currentState = StageState::START;
};
