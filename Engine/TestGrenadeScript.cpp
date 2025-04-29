#include "pch.h"
#include "TestGrenadeScript.h"
#include "Input.h"
#include "Engine.h"
#include "Timer.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "ParticleSystem.h"
#include "Transform.h"

TestGrenadeScript::TestGrenadeScript(shared_ptr<PlayerScript> playerScript)
	: _playerScript(playerScript),
	_throwDelay(0.0f), _pendingThrow(false), _parentTransform(nullptr),
	_grenadeCooldown(10.0f), _grenadeCooldownTimer(0.0f)
{
}

TestGrenadeScript::~TestGrenadeScript()
{
}

void TestGrenadeScript::LateUpdate()
{
	// 쿨타임 갱신
	if (_grenadeCooldownTimer > 0.f)
		_grenadeCooldownTimer -= DELTA_TIME;

	KeyboardInput();
	MouseInput();
	ThrowGrenade();

	if (_resetThrow)
	{
		_isThrown = true;
		_resetThrow = false;
	}
}

void TestGrenadeScript::KeyboardInput()
{
	if (INPUT->GetButtonDown(KEY_TYPE::E) &&
		!_pendingThrow && !_isThrown && _grenadeCooldownTimer <= 0.f)
	{
		if (!(_playerScript->GetIsGrenade())) return;

		GetGameObject()->SetRenderOff();

		// 대기 상태 진입
		_pendingThrow = true;
		_throwDelay = 0.0f;

		// 부모 정보 저장 및 위치 초기화
		GetTransform()->RestoreParent();
		auto parent = GetTransform()->GetParent().lock();
		_parentTransform = parent->GetTransform();

		Vec3 parentPos = _parentTransform->GetLocalPosition();
		parentPos.y += 300.f;
		GetTransform()->SetLocalPosition(parentPos);

		// 초기화
		_velocity = Vec3(0, 0, 0);
		GetTransform()->RemoveParent();

		// 파티클 중지
		auto particle_system = GetGameObject()->GetParticleSystem();
		if (particle_system)
			particle_system->ParticleStop();

		_timeSinceLanded = -1.0f;
	}
}

void TestGrenadeScript::MouseInput()
{
}

void TestGrenadeScript::ThrowGrenade()
{
	if (_pendingThrow)
	{
		_throwDelay += DELTA_TIME;

		if (_throwDelay >= 0.3f)
		{
			_pendingThrow = false;
			_throwDelay = 0.0f;

			if (!_parentTransform)
				return;

			float radian = ToRadian(_angle);
			Vec3 forward = _parentTransform->GetLook();
			forward.y = 0.0f;
			forward.Normalize();

			_velocity = forward * (cos(radian) * _power);
			_velocity.y = sin(radian) * _power;

			_gravity = -9.81f * 200.0f;
			_resetThrow = true;
		}
	}

	if (_isThrown)
	{
		GetGameObject()->SetRenderOn();

		Vec3 pos = GetTransform()->GetLocalPosition();
		_velocity.y += _gravity * DELTA_TIME;
		pos += _velocity * DELTA_TIME;
		GetTransform()->SetLocalPosition(pos);

		if (pos.y <= -70.0f)
		{
			pos.y = -70.0f;
			_isThrown = false;

			// 파티클 재생
			auto particle_system = GetGameObject()->GetParticleSystem();
			if (particle_system)
				particle_system->ParticleStart();

			_timeSinceLanded = 0.0f;

			// 쿨타임 시작
			_grenadeCooldownTimer = _grenadeCooldown;
		}
	}

	if (_timeSinceLanded >= 0.0f)
	{
		_timeSinceLanded += DELTA_TIME;

		if (_timeSinceLanded >= 5.0f)
		{
			GetGameObject()->SetRenderOff();
			_timeSinceLanded = -1.0f;
		}
	}
}