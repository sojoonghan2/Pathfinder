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
#include "light.h"

PlayerScript::PlayerScript() {}
PlayerScript::~PlayerScript() {}

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

	if (_moveTimer.PeekDeltaTime() > MOVE_PACKET_TIME_MS) {
		_moveTimer.updateDeltaTime();
		Vec3 pos = GetTransform()->GetLocalPosition();
		GET_SINGLE(SocketIO)->DoSend<packet::CSMovePlayer>(pos.x, pos.y);
	}
#endif

	_isMove = false;
	KeyboardInput();
	MouseInput();
	RotateToCameraOnShoot();
	ThrowGrenade();
	ShootRazer();
	Animation();

	// 테스트용 HP 코드(함수로 뺄거면 빼시게)
	if (INPUT->GetButton(KEY_TYPE::K))
	{
		auto hpTransform = GET_SINGLE(SceneManager)->FindObjectByName(L"HP")->GetTransform();

		Vec3 hpScale = hpTransform->GetLocalScale();
		Vec3 hpPos = hpTransform->GetLocalPosition();

		float delta = 10.f;

		if (hpScale.x - delta > 0.f)
		{
			hpScale.x -= delta;
			hpPos.x -= delta * 0.5f;

			hpTransform->SetLocalScale(hpScale);
			hpTransform->SetLocalPosition(hpPos);
		}
	}
	if (INPUT->GetButton(KEY_TYPE::L))
	{
		auto hpTransform = GET_SINGLE(SceneManager)->FindObjectByName(L"HP")->GetTransform();

		Vec3 hpScale = hpTransform->GetLocalScale();
		Vec3 hpPos = hpTransform->GetLocalPosition();

		float delta = 10.f;

		hpScale.x += delta;
		hpPos.x += delta * 0.5f;

		hpTransform->SetLocalScale(hpScale);
		hpTransform->SetLocalPosition(hpPos);
	}
	auto light = GET_SINGLE(SceneManager)->FindObjectByName(L"directional")->GetLight();
	if (INPUT->GetButton(KEY_TYPE::KEY_1))
	{
		Vec4 dif = light->GetDiffuse();
		dif.x += 0.1;
		light->SetDiffuse(Vec3(dif.x, dif.y, dif.z));
		cout << "Dif: " << dif.x << ", " << dif.y << ", " << dif.z << "\n";
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_2))
	{
		Vec4 dif = light->GetDiffuse();
		dif.y += 0.1;
		light->SetDiffuse(Vec3(dif.x, dif.y, dif.z));
		cout << "Dif: " << dif.x << ", " << dif.y << ", " << dif.z << "\n";
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_3))
	{
		Vec4 dif = light->GetDiffuse();
		dif.z += 0.1;
		light->SetDiffuse(Vec3(dif.x, dif.y, dif.z));
		cout << "Dif: " << dif.x << ", " << dif.y << ", " << dif.z << "\n";

	}
	if (INPUT->GetButton(KEY_TYPE::KEY_4))
	{
		Vec4 amb = light->GetAmbient();
		amb.x += 0.1;
		light->SetAmbient(Vec3(amb.x, amb.y, amb.z));
		cout << "Amb: " << amb.x << ", " << amb.y << ", " << amb.z << "\n";

	}
	if (INPUT->GetButton(KEY_TYPE::KEY_5))
	{
		Vec4 amb = light->GetAmbient();
		amb.y += 0.1;
		light->SetAmbient(Vec3(amb.x, amb.y, amb.z));
		cout << "Amb: " << amb.x << ", " << amb.y << ", " << amb.z << "\n";

	}
	if (INPUT->GetButton(KEY_TYPE::KEY_6))
	{
		Vec4 amb = light->GetAmbient();
		amb.z += 0.1;
		light->SetAmbient(Vec3(amb.x, amb.y, amb.z));
		cout << "Amb: " << amb.x << ", " << amb.y << ", " << amb.z << "\n";

	}
	if (INPUT->GetButton(KEY_TYPE::KEY_7))
	{
		Vec4 spc = light->GetDiffuse();
		spc.x += 0.1;
		light->SetDiffuse(Vec3(spc.x, spc.y, spc.z));
		cout << "Spc: " << spc.x << ", " << spc.y << ", " << spc.z << "\n";
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_8))
	{
		Vec4 spc = light->GetDiffuse();
		spc.y += 0.1;
		light->SetDiffuse(Vec3(spc.x, spc.y, spc.z));
		cout << "Spc: " << spc.x << ", " << spc.y << ", " << spc.z << "\n";
	}
	if (INPUT->GetButton(KEY_TYPE::KEY_9))
	{
		Vec4 spc = light->GetDiffuse();
		spc.z += 0.1;
		light->SetDiffuse(Vec3(spc.x, spc.y, spc.z));
		cout << "Spc: " << spc.x << ", " << spc.y << ", " << spc.z << "\n";
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
	float minY = 0.f, maxY = 9500.f;

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
		if (_dashCooldownTimer <= 0.f)
			GET_SINGLE(SceneManager)->FindObjectByName(L"DashUI")->SetRenderOn();
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
		GET_SINGLE(SceneManager)->FindObjectByName(L"DashUI")->SetRenderOff();
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

void PlayerScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * 200.f;
	pos.z = z * 200.f;
	GetTransform()->SetLocalPosition(pos);
}

void PlayerScript::RotateToCameraOnShoot()
{
	if (INPUT->GetButton(KEY_TYPE::RBUTTON))
	{
		auto crosshair = GET_SINGLE(SceneManager)->FindObjectByName(L"CrosshairUI");
		crosshair->SetRenderOn();
		RotateToCameraLook();
	}
	if (INPUT->GetButtonUp(KEY_TYPE::RBUTTON))
	{
		auto crosshair = GET_SINGLE(SceneManager)->FindObjectByName(L"CrosshairUI");
		crosshair->SetRenderOff();
	}
}

void PlayerScript::RotateToCameraLook()
{
	shared_ptr<GameObject> cameraObj = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetGameObject();
	Vec3 camLook = cameraObj->GetTransform()->GetLook();
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
	auto player = GetGameObject();
	if (!player) return;
	int index{};
	while (true)
	{
		wstring name = L"dummy" + to_wstring(index++);
		auto dummy = GET_SINGLE(SceneManager)->FindObjectByName(name);
		if (!dummy)
			break;

		bool is_collision = GET_SINGLE(SceneManager)->Collition(player, dummy);
		if (is_collision)
		{
			Vec3 curPos = player->GetTransform()->GetLocalPosition();
			Vec3 dummyPos = dummy->GetTransform()->GetLocalPosition();
			Vec3 dir = (curPos - dummyPos);

			if (dir.LengthSquared() > 0.001f)
			{
				dir.Normalize();
				curPos += dir * 10.f; // 소폭 밀기
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