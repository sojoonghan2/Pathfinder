#pragma once
#include "MonoBehaviour.h"
#include "NetworkTimer.h"

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

	void RotateToCameraOnShoot();
	void RotateToCameraLook();

	void SetPosition(float x, float z);

	float GetRazerCooldown() { return _razerCooldown; }
	float GetRazerCooldownTimer() { return _razerCooldownTimer; }

	bool GetIsMove() { return _isMove; }
	bool GetIsDashing() { return _isDashing; }
	bool GetIsGrenade() { return _isGrenade; }
	bool GetIsRazer() { return _isRazer; }

	void CheckDummyHits();

private:
	bool			_isMove = false;
	bool			_isDashing = false;
	bool			_isGrenade = false;
	bool			_isRazer = false;
	bool			_isShoot = false;

	float			_speed = PLAYER_SPEED_MPS * METER_TO_CLIENT;
	float			_dashDuration = 0.1f;
	float			_dashTimer = 0.f;

	float			_dashCooldown = SKILL_DASH_COOLDOWN_S;
	float			_dashCooldownTimer = 0.f;

	Vec3			_dashDirection = Vec3::Zero;
	float			_dashSpeed = SKILL_DASH_SPEED_MPS * METER_TO_CLIENT;

	float			_grenadeAniDurationTimer = 0.f;
	float			_grenadeCooldown = SKILL_GRENADE_COOLDOWN_S;
	float			_grenadeCooldownTimer = 0.0f;

	float			_razerAniDurationTimer = 0.f;
	float			_razerCooldown = SKILL_RAZER_COOLDOWN_S;
	float			_razerCooldownTimer = 0.0f;

	float			_shootAniDurationTimer = 0.f;

	NetworkTimer	_moveTimer;
	Vec3			_prevPosition = Vec3::Zero;
};
