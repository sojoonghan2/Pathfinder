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

	// �� ��ȯ�� �� ȣ��
	void ApplyModuleAbillities();

protected:
	bool _isStart = false;
	bool _isClear = false;
	bool _isCreatePortal = false;
};
