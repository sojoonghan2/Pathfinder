#pragma once
#include "MonoBehaviour.h"

class PlayerScript : public MonoBehaviour
{
public:
	PlayerScript();
	virtual ~PlayerScript();

	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();
	void Animation();

	void Move();
	void Dash();
	void ThrowGrenade();

	void SetPosition(float x, float z);

private:
	bool			_isMove = false;
	bool			_isDashing = false;
	bool			_isGrenage = false;

	float			_speed = 1000.f;
	float			_dashDuration = 0.1f;
	float			_dashTimer = 0.f;

	float			_dashCooldown = 1.0f;
	float			_dashCooldownTimer = 0.f;

	Vec3			_dashDirection = Vec3::Zero;
	float			_dashSpeed = 10000.f;

	float			_grenadeTimer = 0.f;
};

