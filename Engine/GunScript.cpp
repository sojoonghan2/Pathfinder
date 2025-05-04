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

void GunScript::Start()
{
	_gunFlame = GET_SINGLE(SceneManager)->FindObjectByName(L"GunFlameParticle");
}


void GunScript::Update()
{
	UpdateGunTransformByInput();
	ApplyTransformInstantly();
	FlamePlaying();
}

void GunScript::UpdateGunTransformByInput()
{
	bool moving = INPUT->GetButton(KEY_TYPE::W) || INPUT->GetButton(KEY_TYPE::A) ||
		INPUT->GetButton(KEY_TYPE::S) || INPUT->GetButton(KEY_TYPE::D);

	if (INPUT->GetButton(KEY_TYPE::R)) {
		SetRazerPose();
	}
	else if (INPUT->GetButton(KEY_TYPE::E)) {
		SetGrenadePose();
	}
	else if (INPUT->GetButton(KEY_TYPE::SPACE)) {
		SetDashPose();
	}
	else if (INPUT->GetButton(MOUSE_TYPE::LBUTTON) && moving) {
		SetShootMovePose();
	}
	else if (INPUT->GetButton(MOUSE_TYPE::LBUTTON)) {
		SetShootIdlePose();
	}
	else if (moving) {
		SetMovePose();
	}
	else {
		SetIdlePose();
	}
}

void GunScript::ApplyTransformInstantly()
{
	GetTransform()->SetLocalPosition(_targetPos);
}

void GunScript::FlamePlaying()
{
	if (!_gunFlame) return;

	if (INPUT->GetButton(MOUSE_TYPE::LBUTTON))
	{
		if (!_isFlamePlaying)
		{
			_gunFlame->GetParticleSystem()->ParticleStart();
			_isFlamePlaying = true;
		}
	}
	else
	{
		if (_isFlamePlaying)
		{
			_gunFlame->GetParticleSystem()->ParticleStop();
			_isFlamePlaying = false;
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
