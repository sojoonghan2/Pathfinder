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

	// 카메라 전환
	void ToggleCamera();

private:
	// 마우스 중앙 고정용 위치
	POINT					_centerScreenPos;

	// 카메라 이동 속도
	float					_speed = 10000.f;

	// 카메라 모드
	bool					_playerCamera = true;

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

	Vec3					_defaultOffset = Vec3(0.f, 500.f, -600.f);
	Vec3					_zoomOffset = Vec3(100.f, 300.f, -300.f); // 어깨 너머 줌 위치
	Vec3					_currentOffset = _defaultOffset;
	float					_currentFOV = 60.f;
	float					_targetFOV = 60.f;
};
