#include "pch.h"
#include "CrabScript.h"
#include "Input.h"
#include "Timer.h"
#include "Animator.h"
#include "MeshData.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "ParticleSystem.h"
#include "BaseCollider.h"

CrabScript::CrabScript()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> angleDis(0.0f, 360.0f);
	std::uniform_real_distribution<float> pauseDis(1.0f, 5.0f);

	float angle = angleDis(gen) * (3.141592f / 180.0f);
	_direction.x = cos(angle);
	_direction.z = sin(angle);
	_direction.y = 0.0f;

	_isMoving = false;
	_elapsedTime = 0.0f;
	_pauseDuration = pauseDis(gen);

	_hitTime = 100.f;
	_hitDuration = 0.2f;
}

void CrabScript::Start()
{
	_player = GET_SINGLE(SceneManager)->FindObjectByName(L"Player");

	wstring myName = GetGameObject()->GetName();
	size_t numberPos = myName.find_first_of(L"0123456789");
	if (numberPos != wstring::npos)
		_index = stoi(myName.substr(numberPos));

	wstring hpName = L"CrabHP" + to_wstring(_index);
	_hp = GET_SINGLE(SceneManager)->FindObjectByName(hpName);

	_particlePool.Init(_particleObjects);
}

void CrabScript::LateUpdate()
{
	// 사망 애니메이션 처리
	if (_isDying)
	{
		const float duration = 0.3f;
		_deathAnimTime += DELTA_TIME;

		float t = min(_deathAnimTime / duration, 1.0f);
		float logT = log(1 + 30 * t) / log(10);

		Vec3 currentRot = Vector3::Lerp(_startRot, _targetRot, logT);
		Vec3 currentPos = Vector3::Lerp(_startPos, _targetPos, logT);

		GetTransform()->SetLocalRotation(currentRot);
		GetTransform()->SetLocalPosition(currentPos);

		if (t >= 1.0f) _isDying = false;
		return;
	}

	// 죽었는지 체크
	if (!_isAlive)
	{
		if (!_deathHandled)
			DeadAnimation();
		return;
	}

	_elapsedTime += DELTA_TIME;

	if (_isMoving && _elapsedTime >= 3.0f)
	{
		_isMoving = false;
		_elapsedTime = 0.0f;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> pauseDis(1.0f, 5.0f);
		_pauseDuration = pauseDis(gen);

		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
		int32 index = (currentIndex + 1) % count;
		GetAnimator()->Play(index);
	}

	if (!_isMoving && _elapsedTime >= _pauseDuration)
	{
		_isMoving = true;
		_elapsedTime = 0.0f;

		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
		int32 index = (currentIndex + 1) % count;
		GetAnimator()->Play(index);
	}

	if (_isMoving && _isAlive)
	{
		MoveRandomly();
		CheckBoundary();
	}

	CheckRazerHits();
}

void CrabScript::MoveRandomly()
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos += _direction * _speed * DELTA_TIME;
	GetTransform()->SetLocalPosition(pos);
}

void CrabScript::CheckBoundary()
{
	Vec3 pos = GetTransform()->GetLocalPosition();

	float mapMinX = -4000.f;
	float mapMaxX = 4000.f;
	float mapMinZ = -4000.f;
	float mapMaxZ = 4000.f;

	if (pos.x <= mapMinX || pos.x >= mapMaxX)
	{
		_direction.x *= -1;
		pos.x = max(mapMinX, min(pos.x, mapMaxX));
	}

	if (pos.z <= mapMinZ || pos.z >= mapMaxZ)
	{
		_direction.z *= -1;
		pos.z = max(mapMinZ, min(pos.z, mapMaxZ));
	}

	GetTransform()->SetLocalPosition(pos);
}

void CrabScript::CheckDummyHits(shared_ptr<GameObject> dummy)
{
	auto crab = GetGameObject();
	if (!crab || !dummy) return;

	Vec3 curPos = crab->GetTransform()->GetLocalPosition();
	Vec3 dummyPos = dummy->GetTransform()->GetLocalPosition();

	Vec3 hitDir = curPos - dummyPos;

	if (hitDir.LengthSquared() > 0.0001f)
	{
		hitDir.Normalize();

		// 방향만 반전
		_direction.x = hitDir.x;
		_direction.z = hitDir.z;
	}
	// 강제 탈출
	else
	{
		_direction.x *= -1.f;
		_direction.z *= -1.f;
	}
}

void CrabScript::CheckBulletHits(shared_ptr<GameObject> bullet)
{
	if (!_hp)
		return;

	bullet->GetCollider()->SetEnable(false);
	auto particle = _particlePool.GetAvailable();
	if (particle)
	{
		particle->ParticleStart();
	}

	Vec3 hpScale = _hp->GetTransform()->GetLocalScale();
	Vec3 hpPos = _hp->GetTransform()->GetLocalPosition();
	float delta = 10.f;

	if (_takeGrenade)
		delta *= 2.f;

	if (hpScale.x - delta >= 0.f)
	{
		hpScale.x -= delta;
		hpPos.x -= delta * 0.5f;

		_hp->GetTransform()->SetLocalScale(hpScale);
		_hp->GetTransform()->SetLocalPosition(hpPos);
	}

	if ((hpScale.x - delta) < 0.f)
	{
		DeadAnimation();
		_hp->SetRender(false);
	}
}

void CrabScript::CheckGrenadeHits()
{
	_takeGrenade = true;
	cout << "수류탄 피격\n";
}

void CrabScript::CheckRazerHits()
{
	// 구현 예정
}

void CrabScript::DeadAnimation()
{
	if (!_deathHandled)
	{
		_isAlive = false;
		_deathHandled = true;

		if (GetAnimator())
			GetAnimator()->Stop();

		_startPos = GetTransform()->GetLocalPosition();
		_startRot = GetTransform()->GetLocalRotation();

		Vec3 playerUp = _player->GetTransform()->GetUp();
		Vec3 backward = playerUp;
		_targetPos = _startPos + backward * 1000.f;
		_targetRot = Vec3(180.0f * (PI / 180.0f), _startRot.y, _startRot.z);

		_deathAnimTime = 0.f;
		_isDying = true;
	}
}