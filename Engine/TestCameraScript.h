#pragma once
#include "MonoBehaviour.h"

class TestCameraScript : public MonoBehaviour
{
public:
	TestCameraScript();
	virtual ~TestCameraScript();

	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();

	// ī�޶� ��ȯ
	void ToggleCamera();

private:
	// ī�޶� �̵� �ӵ�
	float					_speed = 10000.f;
	// ī�޶� ���
	bool					_playerCamera = false;
	// ī�޶� ���󰡴� Ÿ�� ������Ʈ
	shared_ptr<GameObject>	_target;
	// Ÿ�ٰ��� �Ÿ�
	Vec3					_offsetPosition;
	float					_rotationY;

};

