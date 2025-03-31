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
	void Shoot();
	void ThrowGrenade();
	void ShootRazer();

	void SetPosition(float x, float z);

	float GetRazerCooldown() { return _razerCooldown; }
	float GetRazerCooldownTimer() { return _razerCooldownTimer; }

	bool GetIsMove() { return _isMove; }
	bool GetIsDashing() { return _isDashing; }
	bool GetIsGrenade() { return _isGrenade; }
	bool GetIsRazer() { return _isRazer; }

private:
	bool			_isMove = false;
	bool			_isDashing = false;
	bool			_isGrenade = false;
	bool			_isRazer = false;
	bool			_isShoot = false;

	float			_speed = 1000.f;
	float			_dashDuration = 0.1f;
	float			_dashTimer = 0.f;

	float			_dashCooldown = 1.0f;
	float			_dashCooldownTimer = 0.f;

	Vec3			_dashDirection = Vec3::Zero;
	float			_dashSpeed = 10000.f;

	float			_grenadeAniDurationTimer = 0.f;
	float			_grenadeCooldown = 10.0f;
	float			_grenadeCooldownTimer = 0.0f;

	float			_razerAniDurationTimer = 0.f;
	float			_razerCooldown = 10.0f;
	float			_razerCooldownTimer = 0.0f;

	float			_shootAniDurationTimer = 0.f;
};

