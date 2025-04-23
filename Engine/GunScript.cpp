#include "pch.h"
#include "GunScript.h"
#include "Transform.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "ParticleSystem.h"
#include "Scene.h"
#include "SceneManager.h"

GunScript::GunScript() {}
GunScript::~GunScript() {}

void GunScript::Update()
{
	UpdateGunTransformByInput();
	ApplyTransformInstantly();
	FlamePlaying();
}

void GunScript::UpdateGunTransformByInput()
{
	int state = 0; // ±âº»: Idle

	if (INPUT->GetButton(KEY_TYPE::W) || INPUT->GetButton(KEY_TYPE::A) || INPUT->GetButton(KEY_TYPE::S) || INPUT->GetButton(KEY_TYPE::D))
		state = 1;
	if (INPUT->GetButton(KEY_TYPE::SPACE))
		state = 2;
	if (INPUT->GetButton(KEY_TYPE::E))
		state = 3;
	if (INPUT->GetButton(KEY_TYPE::LBUTTON) && state == 1)
	{
		state = 5;
	}
	else if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		state = 4;
	}
	if (INPUT->GetButton(KEY_TYPE::R))
		state = 7;

	switch (state)
	{
	case 0: SetIdlePose(); break;
	case 1: SetMovePose(); break;
	case 2: SetDashPose(); break;
	case 3: SetGrenadePose(); break;
	case 4: SetShootIdlePose(); break;
	case 5: SetShootMovePose(); break;
	case 7: SetRazerPose(); break;
	default: SetIdlePose(); break;
	}
}

void GunScript::ApplyTransformInstantly()
{
	GetTransform()->SetLocalPosition(_targetPos);
}

void GunScript::FlamePlaying()
{
	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		if (!_isFlamePlaying)
		{
			auto gunFlame = GET_SINGLE(SceneManager)->FindObjectByName(L"GunFlameParticle");
			if (gunFlame)
			{
				gunFlame->GetParticleSystem()->ParticleStart();
				_isFlamePlaying = true;
			}
		}
	}
	else
	{
		if (_isFlamePlaying)
		{
			auto gunFlame = GET_SINGLE(SceneManager)->FindObjectByName(L"GunFlameParticle");
			if (gunFlame)
			{
				gunFlame->GetParticleSystem()->ParticleStop();
				_isFlamePlaying = false;
			}
		}
	}

}

void GunScript::SetIdlePose()
{
	_idleTime += DELTA_TIME;
	float offsetZ = 1.5f * sinf(_idleTime * 3.1f);

	_targetPos = Vec3(42.f, 58.f, -3.f + offsetZ);
	_targetRot = Vec3(PI / 2, 0.f, 0.f);
}

void GunScript::SetMovePose()
{
	_idleTime = 0.f;
	_targetPos = Vec3(42.f, 65.f, -3.f);
	_targetRot = Vec3(PI / 2, 0.05f, 0.f);
}

void GunScript::SetDashPose()
{
	_idleTime = 0.f;
	_targetPos = Vec3(45.f, 59.f, -3.f);
	_targetRot = Vec3(PI / 2, -0.1f, 0.1f);
}

void GunScript::SetGrenadePose()
{
	_idleTime = 0.f;
	_targetPos = Vec3(38.f, 63.f, 17.f);
	_targetRot = Vec3(PI / 2, 0.3f, -0.1f);
}

void GunScript::SetShootIdlePose()
{
	_idleTime += DELTA_TIME;
	float recoilY = 3.f * sinf(_idleTime * 3.5f);

	_targetPos = Vec3(44.f, 64.f + fabsf(recoilY), 22.f);
	_targetRot = Vec3(PI / 2, 0.1f, 0.f);
}

void GunScript::SetShootMovePose()
{
	_shootMoveTime += DELTA_TIME;

	float t = min(_shootMoveTime * 5.0f, 1.0f);
	float recoilZ = 5.f * sinf((_shootMoveTime - 0.1f) * 7.5f);

	float baseZ = 10.f + (t > 0.2f ? fabsf(recoilZ) : 0.f);

	_targetPos = Vec3(43.f, 70.f, baseZ);
	_targetRot = Vec3(PI / 2, 0.15f, 0.f);
}

void GunScript::SetRazerPose()
{
	_idleTime = 0.f;
	_targetPos = Vec3(35.f, 55.f, 16.f);
	_targetRot = Vec3(PI / 2, -0.3f, 0.2f);
}
