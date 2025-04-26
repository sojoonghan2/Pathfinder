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
#include "BoxCollider.h"
#include "SphereCollider.h"

PlayerScript::PlayerScript() {}
PlayerScript::~PlayerScript() {}

void PlayerScript::Start()
{
	_dashUI = GET_SINGLE(SceneManager)->FindObjectByName(L"DashUI");
	_grenadeUI = GET_SINGLE(SceneManager)->FindObjectByName(L"GrenadeUI");
	_razerUI = GET_SINGLE(SceneManager)->FindObjectByName(L"RazerUI");
	_crosshairUI = GET_SINGLE(SceneManager)->FindObjectByName(L"CrosshairUI");

	auto hpObj = GET_SINGLE(SceneManager)->FindObjectByName(L"HP");
	if (hpObj) _hpTransform = hpObj->GetTransform();

	_cameraObj = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetGameObject();

	// Dummy 캐싱
	int index = 0;
	while (true)
	{
		wstring name = L"dummy" + to_wstring(index++);
		auto dummy = GET_SINGLE(SceneManager)->FindObjectByName(name);
		if (!dummy) break;
		_dummyList.push_back(dummy);
	}
	_dummiesInitialized = true;
}

void PlayerScript::LateUpdate()
{
#ifdef NETWORK_ENABLE
	int id{ GET_SINGLE(SocketIO)->_myId};
	if (-1 != id) {
		auto& queue{ GET_SINGLE(MessageManager)->GetMessageQueue(id) };
		while (not queue.empty()) {
			auto& message{ queue.front() };
			switch (message->type)
			{
			case MsgType::MOVE:
			{
				std::shared_ptr<MsgMove> message_move{
					std::static_pointer_cast<MsgMove>(message) };
				SetPosition(message_move->x, message_move->y);
			}
			break;
			default:
				break;
			}
			queue.pop();
		}
	}

	if (_moveTimer.PeekDeltaTime() > MOVE_PACKET_TIME_MS) {
		_moveTimer.updateDeltaTime();
		Vec3 pos = GetTransform()->GetLocalPosition();
		Vec3 dir = GetTransform()->GetLook();
		dir.y = 0.f;
		dir.Normalize();
		GET_SINGLE(SocketIO)->DoSend<packet::CSMovePlayer>(
			pos.x / METER_TO_CLIENT, pos.z / METER_TO_CLIENT,
			dir.x, dir.z);
	}
#endif

	_isMove = false;
	KeyboardInput();
	MouseInput();
	RotateToCameraOnShoot();
	ThrowGrenade();
	ShootRazer();
	Animation();
	Recoil();

	// 테스트용 HP 코드
	if (_hpTransform && INPUT->GetButton(KEY_TYPE::K))
	{
		Vec3 scale = _hpTransform->GetLocalScale();
		Vec3 pos = _hpTransform->GetLocalPosition();

		float delta = 10.f;
		if (scale.x - delta >= 0.f)
		{
			scale.x -= delta;
			pos.x -= delta * 0.41f;
			_hpTransform->SetLocalScale(scale);
			_hpTransform->SetLocalPosition(pos);
		}
	}
}

void PlayerScript::KeyboardInput() { Move(); Dash(); }
void PlayerScript::MouseInput() { Shoot(); }

void PlayerScript::Animation()
{
	static uint32 currentAnimIndex = 0;
	uint32 nextAnimIndex = 0;

	if (_isRazer)
		nextAnimIndex = 7;
	else if (_isGrenade)
		nextAnimIndex = 3;
	else if (_isDashing)
		nextAnimIndex = 2;
	else if (_isShoot && _isMove)
		nextAnimIndex = 5;
	else if (_isShoot && !_isMove)
		nextAnimIndex = 4;
	else if (_isMove)
		nextAnimIndex = 1;
	else
		nextAnimIndex = 0;

	if (currentAnimIndex != nextAnimIndex)
	{
		GetAnimator()->Play(nextAnimIndex);
		currentAnimIndex = nextAnimIndex;
	}
}

void PlayerScript::Move()
{
	if (_isDashing || _isGrenade || _isRazer) return;

	_prevPosition = GetTransform()->GetLocalPosition();
	Vec3 pos = _prevPosition;

	Vec3 camForward = _cameraObj->GetTransform()->GetLook();
	Vec3 camRight = _cameraObj->GetTransform()->GetRight();
	camForward.y = 0;
	camRight.y = 0;
	camForward.Normalize();
	camRight.Normalize();

	Vec3 moveDir = Vec3(0.f);

	if (INPUT->GetButton(KEY_TYPE::W)) moveDir += camForward;
	if (INPUT->GetButton(KEY_TYPE::S)) moveDir -= camForward;
	if (INPUT->GetButton(KEY_TYPE::A)) moveDir -= camRight;
	if (INPUT->GetButton(KEY_TYPE::D)) moveDir += camRight;
	if (INPUT->GetButton(KEY_TYPE::T)) PRINTPOSITION;

	if (moveDir.LengthSquared() > 0.01f)
	{
		moveDir.Normalize();
		pos += moveDir * _speed * DELTA_TIME;
		_isMove = true;

		float targetYaw = atan2f(moveDir.x, moveDir.z) + XM_PI;
		Vec3 currentRot = GetTransform()->GetLocalRotation();
		Vec3 targetRot = Vec3(-XM_PIDIV2, targetYaw, 0.f);

		float deltaYaw = targetRot.y - currentRot.y;
		if (deltaYaw > XM_PI) deltaYaw -= XM_2PI;
		else if (deltaYaw < -XM_PI) deltaYaw += XM_2PI;

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

	float mapMinX = -4950.f, mapMaxX = 4950.f;
	float mapMinZ = -4950.f, mapMaxZ = 4950.f;
	float minY = -100.f, maxY = 9500.f;

	pos.x = max(mapMinX, min(pos.x, mapMaxX));
	pos.y = max(minY, min(pos.y, maxY));
	pos.z = max(mapMinZ, min(pos.z, mapMaxZ));

	GetTransform()->SetLocalPosition(pos);
	CheckDummyHits();
}

void PlayerScript::Dash()
{
	if (_isGrenade || _isRazer) return;

	if (_dashCooldownTimer > 0.f)
	{
		_dashCooldownTimer -= DELTA_TIME;
		if (_dashCooldownTimer <= 0.f && _dashUI)
			_dashUI->SetRenderOn();
	}

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
		Vec3 lookDir = GetTransform()->GetLook();
		lookDir.y = 0.f;
		lookDir.Normalize();

		_dashDirection = lookDir;
		_isDashing = true;
		_dashTimer = _dashDuration;
		_dashUI->SetRenderOff();
	}
}

void PlayerScript::Shoot()
{
	if (_isGrenade || _isRazer || _isDashing) return;

	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		if (!_isShoot)
		{
			_isShoot = true;
			_shootAniDurationTimer = 1.0f;
		}
		else
		{
			_shootAniDurationTimer = 1.0f;
		}

		ShakeCamera();
	}
	else
	{
		_isShoot = false;
		_shootAniDurationTimer = 0.f;
	}
}

void PlayerScript::ThrowGrenade()
{
	if (_isDashing || _isRazer) return;

	if (_grenadeCooldownTimer > 0.f)
	{
		_grenadeCooldownTimer -= DELTA_TIME;
		if (_grenadeCooldownTimer <= 0.f)
			GET_SINGLE(SceneManager)->FindObjectByName(L"GrenadeUI")->SetRenderOn();
	}

	if (_isGrenade)
	{
		_grenadeAniDurationTimer -= DELTA_TIME;
		if (_grenadeAniDurationTimer <= 0.f)
		{
			_isGrenade = false;
			_grenadeAniDurationTimer = 0.f;
			_grenadeCooldownTimer = _grenadeCooldown;
		}
		return;
	}

	if (_grenadeCooldownTimer <= 0.f && INPUT->GetButtonDown(KEY_TYPE::E))
	{
		RotateToCameraLook();
		_isGrenade = true;
		_grenadeAniDurationTimer = 3.0f;
		GET_SINGLE(SceneManager)->FindObjectByName(L"GrenadeUI")->SetRenderOff();
	}
}

void PlayerScript::ShootRazer()
{
	if (_isDashing || _isGrenade) return;

	if (_razerCooldownTimer > 0.f)
	{
		_razerCooldownTimer -= DELTA_TIME;
		if (_razerCooldownTimer <= 0.f)
			GET_SINGLE(SceneManager)->FindObjectByName(L"RazerUI")->SetRenderOn();
	}

	if (_isRazer)
	{
		_razerAniDurationTimer -= DELTA_TIME;
		if (_razerAniDurationTimer <= 0.f)
		{
			_isRazer = false;
			_razerAniDurationTimer = 0.f;
			_razerCooldownTimer = _razerCooldown;
		}
		return;
	}

	if (_razerCooldownTimer <= 0.f && INPUT->GetButtonDown(KEY_TYPE::R))
	{
		RotateToCameraLook();
		_isRazer = true;
		_razerAniDurationTimer = 7.5f;
		GET_SINGLE(SceneManager)->FindObjectByName(L"RazerUI")->SetRenderOff();
	}
}

void PlayerScript::ShakeCamera()
{
	_recoilAccumulation = min(_recoilAccumulation + 0.2f, _maxRecoil);

	Vec3 playerLook = GetTransform()->GetUp();
	playerLook.Normalize();

	float recoilAmount = _horizontalRecoil * (1.0f + _recoilAccumulation * 0.2f);
	float shakeStrength = (rand() % 2000 / 1000.0f - 1.0f) * recoilAmount;
	Vec3 shakeOffset = playerLook * shakeStrength;
	_cameraShakeOffset += shakeOffset;

	Vec3 camPos = _cameraObj->GetTransform()->GetLocalPosition();
	_cameraObj->GetTransform()->SetLocalPosition(camPos + _cameraShakeOffset);
}

void PlayerScript::Recoil()
{
	// 카메라 흔들림 감소 처리
	if (_cameraShakeOffset.LengthSquared() > 0.001f)
	{
		// 카메라 위치 복구
		if (_cameraObj != nullptr)
		{
			Vec3 camPos = _cameraObj->GetTransform()->GetLocalPosition();
			_cameraObj->GetTransform()->SetLocalPosition(camPos - _cameraShakeOffset);

			// 흔들림 감소
			_cameraShakeOffset *= (1.0f - _cameraShakeDecay * DELTA_TIME);

			// 매우 작은 값이면 0으로 설정
			if (_cameraShakeOffset.LengthSquared() < 0.001f)
				_cameraShakeOffset = Vec3(0.f);

			// 수정된 흔들림 적용
			_cameraObj->GetTransform()->SetLocalPosition(camPos - _cameraShakeOffset + _cameraShakeOffset);
		}
	}

	// 사격을 멈추면 반동 누적치 감소
	if (!_isShoot)
	{
		_recoilAccumulation = max(0.0f, _recoilAccumulation - 0.5f * DELTA_TIME);
	}
}

void PlayerScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * 200.f;
	pos.z = z * 200.f;
	GetTransform()->SetLocalPosition(pos);
}


void PlayerScript::RotateToCameraOnShoot()
{
	if (_crosshairUI)
	{
		if (INPUT->GetButton(KEY_TYPE::RBUTTON))
		{
			_crosshairUI->SetRenderOn();
			RotateToCameraLook();
		}
		if (INPUT->GetButtonUp(KEY_TYPE::RBUTTON))
		{
			_crosshairUI->SetRenderOff();
		}
	}

}

void PlayerScript::RotateToCameraLook()
{
	Vec3 camLook = _cameraObj->GetTransform()->GetLook();
	camLook.y = 0.f;
	camLook.Normalize();

	if (camLook.LengthSquared() > 0.0001f)
	{
		float targetYaw = atan2f(camLook.x, camLook.z) + XM_PI;
		Vec3 targetRot = Vec3(-XM_PIDIV2, targetYaw, 0.f);
		GetTransform()->SetLocalRotation(targetRot);
	}
}

void PlayerScript::CheckDummyHits()
{
	if (!_dummiesInitialized) return;

	auto player = GetGameObject();
	if (!player) return;

	for (auto& dummy : _dummyList)
	{
		if (!dummy) continue;

		bool is_collision = GET_SINGLE(SceneManager)->Collition(player, dummy);
		if (is_collision)
		{
			Vec3 curPos = player->GetTransform()->GetLocalPosition();
			Vec3 dummyPos = dummy->GetTransform()->GetLocalPosition();
			Vec3 dir = (curPos - dummyPos);

			if (dir.LengthSquared() > 0.001f)
			{
				dir.Normalize();
				curPos += dir * 10.f;
			}
			else
			{
				curPos = _prevPosition;
			}

			player->GetTransform()->SetLocalPosition(curPos);
			break;
		}
	}
}
