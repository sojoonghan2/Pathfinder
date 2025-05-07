#include "pch.h"
#include "GeneratorScript.h"
#include "Input.h"
#include "Animator.h"
#include "GameObject.h"
#include "IceParticleSystem.h"
#include "Timer.h"
#include "SceneManager.h"
#include "BaseCollider.h"

GeneratorScript::GeneratorScript() : _isDead(false), _init(false)
{
}

void GeneratorScript::Start()
{

	_hp = GET_SINGLE(SceneManager)->FindObjectByName(L"GeneratorHP");
	_generatorParticle = GET_SINGLE(SceneManager)->FindObjectByName(L"GeneratorParticle");
}

void GeneratorScript::LateUpdate()
{
	if (!_init)
	{
		if (GetAnimator()) GetAnimator()->Stop();
		_init = true;
	}

	if (_isDead)
	{
		_deadTime += DELTA_TIME;
	}

	// 애니메이션이 끝났으면 렌더링 종료
	if (_deadTime > 7.f)
	{
		GetGameObject()->SetRender(false);
		_isDead = false;
	}

	if(!_isDead) Recovery();
}

void GeneratorScript::CheckBulletHits(shared_ptr<GameObject> bullet)
{
	if (!_hp)
		return;

	bullet->GetCollider()->SetEnable(false);
	_hitTime = 0.f;

	Vec3 hpScale = _hp->GetTransform()->GetLocalScale();
	Vec3 hpPos = _hp->GetTransform()->GetLocalPosition();
	float delta = 10.f;

	if (hpScale.x - delta >= 0.f)
	{
		hpScale.x -= delta;
		hpPos.x -= delta * 0.5f;

		_hp->GetTransform()->SetLocalScale(hpScale);
		_hp->GetTransform()->SetLocalPosition(hpPos);
	}

	_recoveryElapsedTime = 0.f;
	_wasHit = true;

	if ((hpScale.x - delta) < 0.f)
	{
		if (!_isDead)
		{
			if (GetAnimator()) GetAnimator()->Play(0);
			_isDead = true;
			_deadTime = 0.f;
			_generatorParticle->GetParticleSystem()->ParticleStart();
		}
		_hp->SetRender(false);
	}
}

void GeneratorScript::Recovery()
{
	if (_wasHit)
	{
		_recoveryElapsedTime += DELTA_TIME;

		if (_recoveryElapsedTime >= _recoveryWaitTime)
		{
			Vec3 hpScale = _hp->GetTransform()->GetLocalScale();
			Vec3 hpPos = _hp->GetTransform()->GetLocalPosition();
			float delta = 1.f;

			if (hpScale.x + delta <= _hpMax)
			{
				hpScale.x += delta;
				hpPos.x += delta * 0.5f;

				_hp->GetTransform()->SetLocalScale(hpScale);
				_hp->GetTransform()->SetLocalPosition(hpPos);
			}
			else
			{
				hpScale.x = _hpMax;
				_wasHit = false;
			}
			_hp->GetTransform()->SetLocalScale(hpScale);
			_hp->GetTransform()->SetLocalPosition(hpPos);
		}
	}
}