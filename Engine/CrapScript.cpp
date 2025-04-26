#include "pch.h"
#include "CrapScript.h"
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

CrapScript::CrapScript()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> angleDis(0.0f, 360.0f);
	std::uniform_real_distribution<float> pauseDis(1.0f, 5.0f); // 정지 시간 범위: 1초 ~ 5초

	float angle = angleDis(gen) * (3.141592f / 180.0f);
	_direction.x = cos(angle);
	_direction.z = sin(angle);
	_direction.y = 0.0f;

	_isMoving = false;
	_elapsedTime = 0.0f;
	_pauseDuration = pauseDis(gen); // 처음 정지 상태의 랜덤 시간 설정
}

void CrapScript::LateUpdate()
{
	if (_isDying)
	{
		const float duration = 0.5f;
		_deathAnimTime += DELTA_TIME;

		float t = min(_deathAnimTime / duration, 1.0f);
		Vec3 currentRot = Vector3::Lerp(_startRot, _targetRot, t);
		GetTransform()->SetLocalRotation(currentRot);

		if (t >= 1.0f)
			_isDying = false;

		return;
	}

	if (!_isAlive)
	{
		if (!_deathHandled)
			DeadAnimation();
		return;
	}

	_elapsedTime += DELTA_TIME;

	// 이동 상태일 때는 일정 시간 후 정지
	if (_isMoving && _elapsedTime >= 3.0f) // 이동 시간 고정 3초
	{
		_isMoving = false;
		_elapsedTime = 0.0f;

		// 정지 상태 지속 시간 랜덤 설정 (1~5초)
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> pauseDis(1.0f, 5.0f);
		_pauseDuration = pauseDis(gen);

		// 애니메이션을 IDLE로 변경
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
		int32 index = (currentIndex + 1) % count;
		GetAnimator()->Play(index);
	}

	// 정지 상태일 때는 랜덤한 시간 후 이동
	if (!_isMoving && _elapsedTime >= _pauseDuration)
	{
		_isMoving = true;
		_elapsedTime = 0.0f;

		// 애니메이션을 WALK로 변경
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

	CheckBulletHits();
}

void CrapScript::Start()
{
	_bullets.resize(50);
	for (int i = 0; i < 50; ++i)
	{
		wstring name = L"Bullet" + to_wstring(i);
		_bullets[i] = GET_SINGLE(SceneManager)->FindObjectByName(name);
	}

	wstring myName = GetGameObject()->GetName();
	size_t numberPos = myName.find_first_of(L"0123456789");
	if (numberPos != wstring::npos)
	{
		_index = stoi(myName.substr(numberPos));
	}

	wstring hpName = L"CrapHP" + to_wstring(_index);
	auto hpObj = GET_SINGLE(SceneManager)->FindObjectByName(hpName);
	if (hpObj)
		_hpTransform = hpObj->GetTransform();
}

void CrapScript::MoveRandomly()
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos += _direction * _speed * DELTA_TIME;
	GetTransform()->SetLocalPosition(pos);
}

void CrapScript::CheckBoundary()
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

void CrapScript::CheckBulletHits()
{
	if (!_hpTransform) return;

	for (const auto& bullet : _bullets)
	{
		if (!bullet) continue;

		if (GET_SINGLE(SceneManager)->Collition(GetGameObject(), bullet))
		{
			if (!_initialized)
			{
				_initialized = true;
				return;
			}

			if (GetGameObject()->GetParticleSystem())
				GetGameObject()->GetParticleSystem()->ParticleStart();

			Vec3 hpScale = _hpTransform->GetLocalScale();
			Vec3 hpPos = _hpTransform->GetLocalPosition();
			float delta = 10.f;

			if (hpScale.x - delta >= 0.f)
			{
				hpScale.x -= delta;
				hpPos.x -= delta * 0.41f;

				_hpTransform->SetLocalScale(hpScale);
				_hpTransform->SetLocalPosition(hpPos);
			}

			if ((hpScale.x - delta) < 0.f)
			{
				DeadAnimation();
			}
			break;
		}
	}
}

void CrapScript::DeadAnimation()
{
	cout << "응애 거미 사망\n";
	_isAlive = false;

	if (!_deathHandled)
	{
		_deathHandled = true;

		if (GetAnimator())
			GetAnimator()->Stop();

		// 현재 회전값 저장
		_startRot = GetTransform()->GetLocalRotation();
		_targetRot = Vec3(180.0f * (PI / 180.0f), _startRot.y, _startRot.z);
		_deathAnimTime = 0.f;
		_isDying = true;
	}
}