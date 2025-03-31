#include "pch.h"
#include "PlayerScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "MessageManager.h"
#include "SocketIO.h"
#include "Animator.h"

PlayerScript::PlayerScript()
{
}

PlayerScript::~PlayerScript()
{
}

void PlayerScript::LateUpdate()
{
#ifdef NETWORK_ENABLE
	int id = GET_SINGLE(SocketIO)->myId;
	if (-1 != id) {
		auto& queue = GET_SINGLE(MessageManager)->GetMessageQueue(id);
		while (not queue.empty()) {
			auto& message = queue.front();
			SetPosition(message.first, message.second);
			queue.pop();
		}
	}
#endif // NETWORK_ENABLE

	_isMove = false;
	KeyboardInput();
	MouseInput();
	ThrowGrenade();
	ShootRazer();
	Animation();
}

void PlayerScript::KeyboardInput()
{
	Move();
	Dash();
	Shoot();
}

void PlayerScript::MouseInput()
{

}

void PlayerScript::Animation()
{
	static uint32 currentAnimIndex = 0;
	uint32 nextAnimIndex = 0;
	if (_isRazer)
	{
		nextAnimIndex = 7;
	}
	else if (_isShoot && _isMove)
	{
		nextAnimIndex = 5;
	}
	else if (_isShoot && !_isMove)
	{
		nextAnimIndex = 4;
	}
	else if (_isGrenade)
	{
		nextAnimIndex = 3;
	}
	else if (_isDashing)
	{
		nextAnimIndex = 2;
	}
	else if (_isMove)
	{
		nextAnimIndex = 1;
	}
	else
	{
		nextAnimIndex = 0;
	}

	if (currentAnimIndex != nextAnimIndex)
	{
		GetAnimator()->Play(nextAnimIndex);
		currentAnimIndex = nextAnimIndex;
	}
}

void PlayerScript::Move()
{
	if (_isDashing || _isGrenade || _isRazer) return;

	Vec3 pos = GetTransform()->GetLocalPosition();

	shared_ptr<GameObject> cameraObj = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetGameObject();
	Vec3 camForward = cameraObj->GetTransform()->GetLook();
	Vec3 camRight = cameraObj->GetTransform()->GetRight();
	camForward.y = 0;
	camRight.y = 0;
	camForward.Normalize();
	camRight.Normalize();

	Vec3 moveDir = Vec3(0.f);

	if (INPUT->GetButton(KEY_TYPE::W)) moveDir += camForward;
	if (INPUT->GetButton(KEY_TYPE::S)) moveDir -= camForward;
	if (INPUT->GetButton(KEY_TYPE::A)) moveDir -= camRight;
	if (INPUT->GetButton(KEY_TYPE::D)) moveDir += camRight;

	if (moveDir.LengthSquared() > 0.01f)
	{
		moveDir.Normalize();
		pos += moveDir * _speed * DELTA_TIME;
		_isMove = true;

		float targetYaw = atan2f(moveDir.x, moveDir.z) + XM_PI;
		Vec3 currentRot = GetTransform()->GetLocalRotation();
		Vec3 targetRot = Vec3(-XM_PIDIV2, targetYaw, 0.f);

		float deltaYaw = targetRot.y - currentRot.y;
		if (deltaYaw > XM_PI)
			deltaYaw -= XM_2PI;
		else if (deltaYaw < -XM_PI)
			deltaYaw += XM_2PI;

		float lerpSpeed = 10.0f;
		currentRot.y += deltaYaw * lerpSpeed * DELTA_TIME;

		currentRot.y = fmod(currentRot.y, XM_2PI);
		if (currentRot.y < 0.f)
			currentRot.y += XM_2PI;

		GetTransform()->SetLocalRotation(currentRot);
	}
	else
	{
		_isMove = false;
	}

	float mapMinX = -4950.f;
	float mapMaxX = 4950.f;
	float mapMinZ = -4950.f;
	float mapMaxZ = 4950.f;
	float minY = 0.f;
	float maxY = 9500.f;

	pos.x = max(mapMinX, min(pos.x, mapMaxX));
	pos.y = max(minY, min(pos.y, maxY));
	pos.z = max(mapMinZ, min(pos.z, mapMaxZ));

	GetTransform()->SetLocalPosition(pos);
}

void PlayerScript::Dash()
{
	if (_isGrenade || _isRazer) return;

	if (_dashCooldownTimer > 0.f)
		_dashCooldownTimer -= DELTA_TIME;

	if (_isDashing)
	{
		Vec3 pos = GetTransform()->GetLocalPosition();
		pos += _dashDirection * _dashSpeed * DELTA_TIME;
		GetTransform()->SetLocalPosition(pos);

		_dashTimer -= DELTA_TIME;
		if (_dashTimer <= 0.f)
		{
			_isDashing = false;
			_dashCooldownTimer = _dashCooldown;
		}
		return;
	}

	if (_dashCooldownTimer <= 0.f && INPUT->GetButtonDown(KEY_TYPE::SPACE))
	{
		// 플레이어가 바라보는 방향으로 대쉬
		Vec3 lookDir = GetTransform()->GetLook();
		lookDir.y = 0.f;
		lookDir.Normalize();

		_dashDirection = lookDir;
		_isDashing = true;
		_dashTimer = _dashDuration;
	}
}
void PlayerScript::Shoot()
{
	if (_isGrenade || _isRazer || _isDashing) return;

	if (_isShoot)
	{
		_shootAniDurationTimer -= DELTA_TIME;

		if (_shootAniDurationTimer <= 0.f)
		{
			_isShoot = false;
			_shootAniDurationTimer = 0.f;
		}
		return;
	}

	if (_shootAniDurationTimer <= 0.f && INPUT->GetButtonDown(KEY_TYPE::Q))
	{
		_isShoot = true;
		_shootAniDurationTimer = 1.0f;
	}
}
void PlayerScript::ThrowGrenade()
{
	if (_isDashing || _isRazer) return;

	// 쿨타임 갱신
	if (_grenadeCooldownTimer > 0.f)
		_grenadeCooldownTimer -= DELTA_TIME;

	if (_isGrenade)
	{
		_grenadeAniDurationTimer -= DELTA_TIME;

		if (_grenadeAniDurationTimer <= 0.f)
		{
			_isGrenade = false;
			_grenadeAniDurationTimer = 0.f;

			// 쿨타임 시작
			_grenadeCooldownTimer = _grenadeCooldown;
		}
		return;
	}

	if (_grenadeCooldownTimer <= 0.f && INPUT->GetButtonDown(KEY_TYPE::E))
	{
		_isGrenade = true;
		_grenadeAniDurationTimer = 3.0f;
	}
}

void PlayerScript::ShootRazer()
{
	if (_isDashing || _isGrenade) return;

	// 쿨타임 갱신
	if (_razerCooldownTimer > 0.f)
		_razerCooldownTimer -= DELTA_TIME;

	if (_isRazer)
	{
		_razerAniDurationTimer -= DELTA_TIME;

		if (_razerAniDurationTimer <= 0.f)
		{
			_isRazer = false;
			_razerAniDurationTimer = 0.f;

			// 쿨타임 시작
			_razerCooldownTimer = _razerCooldown;
		}
		return;
	}

	if (_razerCooldownTimer <= 0.f && INPUT->GetButtonDown(KEY_TYPE::R))
	{
		_isRazer = true;
		_razerAniDurationTimer = 7.5f;
	}
}

void PlayerScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * 200.f;
	pos.z = z * 200.f;

	GetTransform()->SetLocalPosition(pos);
}
