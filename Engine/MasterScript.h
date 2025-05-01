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

	// �� ��ȯ�� �� ȣ��
	void ApplyModuleAbillities();

protected:
	StageState _currentState = StageState::START;
};
