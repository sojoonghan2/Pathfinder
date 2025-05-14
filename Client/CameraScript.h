#pragma once
#include "MonoBehaviour.h"

class CameraScript : public MonoBehaviour
{
public:
	CameraScript();
	virtual ~CameraScript();

	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();

	// ī�޶� ��ȯ
	void ToggleCamera();

private:
	// ���콺 �߾� ������ ��ġ
	POINT					_centerScreenPos;

	// ī�޶� �̵� �ӵ�
	float					_speed = 10000.f;

	// ī�޶� ���
	bool					_playerCamera = true;

	// ī�޶� ���󰡴� Ÿ�� ������Ʈ
	shared_ptr<GameObject>	_target;

	// Ÿ�ٰ��� �Ÿ�
	Vec3					_offsetPosition;
	Vec3					_cameraRotation;

	Vec3					_revolution;
	float					_tempxRotation = 0.0f;
	float					_tempyRotation = 0.0f;
	float					_yaw = 0.f;
	float					_pitch = 0.f;

	Vec3					_defaultOffset = Vec3(0.f, 500.f, -600.f);
	Vec3					_zoomOffset = Vec3(100.f, 300.f, -300.f); // ��� �ʸ� �� ��ġ
	Vec3					_currentOffset = _defaultOffset;
	float					_currentFOV = 60.f;
	float					_targetFOV = 60.f;
};
