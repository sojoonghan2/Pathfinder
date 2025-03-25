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

	// 카메라 전환
	void ToggleCamera();

private:
	// 카메라 이동 속도
	float					_speed = 10000.f;

	// 카메라 모드
	bool					_playerCamera = false;

	// 카메라가 따라가는 타겟 오브젝트
	shared_ptr<GameObject>	_target;

	// 타겟과의 거리
	Vec3					_offsetPosition;
	Vec3					_cameraRotation;

	Vec3					_revolution;
	float					_tempxRotation = 0.0f;
	float					_tempyRotation = 0.0f;
	float					_yaw = 0.f;
	float					_pitch = 0.f;
};

