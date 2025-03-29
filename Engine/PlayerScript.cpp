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
	Animation();
}

void PlayerScript::KeyboardInput()
{
	Move();
	Dash();
}

void PlayerScript::MouseInput()
{
	// 추후 구현 가능
}

void PlayerScript::Animation()
{
	static uint32 currentAnimIndex = 0;
	uint32 nextAnimIndex = 0;

	if (_isGrenage) {
		nextAnimIndex = 3;
	}
	else if (_isDashing) {
		nextAnimIndex = 2;
	}
	else if (_isMove) {
		nextAnimIndex = 1;
	}
	else {
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
	if (_isDashing || _isGrenage)
		return;

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
	if (_isGrenage)
		return;

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
		shared_ptr<GameObject> cameraObj = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetGameObject();
		Vec3 camForward = cameraObj->GetTransform()->GetLook();
		camForward.y = 0.f;
		camForward.Normalize();

		_dashDirection = camForward;
		_isDashing = true;
		_dashTimer = _dashDuration;
	}
}

void PlayerScript::ThrowGrenade()
{
	if (_isGrenage)
	{
		_grenadeTimer -= DELTA_TIME;

		if (_grenadeTimer <= 0.f)
		{
			_isGrenage = false;
			_grenadeTimer = 0.f;
		}
		return;
	}

	if (INPUT->GetButtonDown(KEY_TYPE::E))
	{
		_isGrenage = true;
		_grenadeTimer = 3.0f;
	}
}

void PlayerScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * 200.f;
	pos.z = z * 200.f;

	GetTransform()->SetLocalPosition(pos);
}
