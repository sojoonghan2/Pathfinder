#pragma once
#include "MonoBehaviour.h"
#include "NetworkTimer.h"

class PlayerScript : public MonoBehaviour
{
public:
	PlayerScript();
	virtual ~PlayerScript();

	virtual void Awake() override;
	virtual void Start() override;
	virtual void LateUpdate() override;

	void KeyboardInput();
	void MouseInput();
	void Animation();

	void Move();
	void Dash();
	void Shoot();
	void ThrowGrenade();
	void ShootRazer();

	void ShakeCamera();
	void Recoil();

	void RotateToCameraOnShoot();
	void RotateToCameraLook();

	void SetPosition(const float x, const float z);

	float GetRazerCooldown() { return _razerCooldown; }
	float GetRazerCooldownTimer() { return _razerCooldownTimer; }

	bool GetIsMove() { return _isMove; }
	bool GetIsDashing() { return _isDashing; }
	bool GetIsGrenade() { return _isGrenade; }
	bool GetIsRazer() { return _isRazer; }

	void CheckDummyHits(shared_ptr<GameObject> dummy);
	void CheckCrabHits();

private:
	bool			_isMove = false;
	bool			_isDashing = false;
	bool			_isGrenade = false;
	bool			_isRazer = false;
	bool			_isShoot = false;

	float			_speed = PLAYER_SPEED_MPS * METER_TO_CLIENT;
	float			_dashDuration = 0.1f;
	float			_dashTimer = 0.f;

	float			_dashCooldown = PLAYER_DASH_COOLDOWN_S;
	float			_dashCooldownTimer = 0.f;

	Vec3			_dashDirection = Vec3::Zero;
	float			_dashSpeed = PLAYER_DASH_SPEED_MPS * METER_TO_CLIENT;

	float			_grenadeAniDurationTimer = 0.f;
	float			_grenadeCooldown = PLAYER_GRENADE_COOLDOWN_S;
	float			_grenadeCooldownTimer = 0.0f;

	float			_razerAniDurationTimer = 0.f;
	float			_razerCooldown = PLAYER_RAZER_COOLDOWN_S;
	float			_razerCooldownTimer = 0.0f;

	float			_shootAniDurationTimer = 0.f;

	NetworkTimer	_moveTimer;
	Vec3			_prevPosition = Vec3::Zero;

	Vec3			_cameraShakeOffset = Vec3(0.f);
	float			_cameraShakeDecay = 10.0f;
	float			_verticalRecoil = 5.0f;
	float			_horizontalRecoil = 5.0f;
	float			_forwardRecoil = 7.0f;
	float			_recoilAccumulation = 0.0f;
	const float		_maxRecoil = 3.0f;

	// UI
	shared_ptr<GameObject> _dashUI;
	shared_ptr<GameObject> _grenadeUI;
	shared_ptr<GameObject> _razerUI;
	shared_ptr<GameObject> _crosshairUI;

	// HP
	shared_ptr<Transform> _hpTransform;

	// Ä«¸Þ¶ó
	shared_ptr<GameObject> _cameraObj;
};
