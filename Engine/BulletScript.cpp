#include "pch.h"
#include "BulletScript.h"
#include "Input.h"
#include "Engine.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "BaseCollider.h"

BulletScript::BulletScript(shared_ptr<PlayerScript> playerScript)
	: _playerScript(playerScript)
{
	_myIndex = s_bulletCount++;
	if (s_bulletCount >= 50)
		s_bulletCount = 0;
}

BulletScript::~BulletScript()
{
}

void BulletScript::Update()
{
	MouseInput();
	MoveBullet();
}

void BulletScript::MouseInput()
{
	if (INPUT->GetButton(MOUSE_TYPE::LBUTTON))
	{
		s_lastFireTime += DELTA_TIME;
		if (!_isFired && _myIndex == s_currentBulletIndex && s_lastFireTime >= s_fireInterval)
		{
			s_lastFireTime = 0.f;
			s_currentBulletIndex = (s_currentBulletIndex + 1) % 50;
			GetGameObject()->SetRender(false);
			_isFired = true;
			_lifeTime = 3.0f;
			GetTransform()->RestoreParent();
			auto parent = GetTransform()->GetParent().lock();
			_parentTransform = parent->GetTransform();
			Vec3 parentPos = _parentTransform->GetLocalPosition();
			parentPos.y += 450.f;
			Vec3 up = _parentTransform->GetUp();
			parentPos += up * 100.f;
			GetTransform()->SetLocalPosition(parentPos);
			GetTransform()->RemoveParent();

			Vec3 dir;

			// ������ ��ư�� ���������� (�� �����̸�) ī�޶� Look ������ ���
			if (INPUT->GetButton(MOUSE_TYPE::RBUTTON))
			{
				auto currentScene = GET_SINGLE(SceneManager)->GetActiveScene();
				auto mainCamera = currentScene->GetMainCamera();
				dir = mainCamera->GetTransform()->GetLook();
			}
			// �ƴϸ� ����ó�� �θ� ��ü�� Look ������ ���
			else
			{
				dir = _parentTransform->GetLook();
				dir.y = 0.0f;

				Vec3 up = _parentTransform->GetUp();
				dir += up * 400.f;
			}

			dir.Normalize();
			_velocity = dir * 20000.f;
		}
	}
	else
	{
		s_lastFireTime = s_fireInterval;
	}
}

void BulletScript::MoveBullet()
{
	if (_isFired)
	{
		GetGameObject()->SetRender(true);
		GetGameObject()->GetCollider()->SetEnable(true);
		Vec3 pos = GetTransform()->GetLocalPosition();
		pos += _velocity * DELTA_TIME;
		GetTransform()->SetLocalPosition(pos);
		_lifeTime -= DELTA_TIME;
		if (_lifeTime <= 0.f)
		{
			_isFired = false;
			GetGameObject()->SetRender(false);
			GetGameObject()->GetCollider()->SetEnable(false);
		}
	}
}