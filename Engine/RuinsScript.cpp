#include "pch.h"
#include "RuinsScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Timer.h"
#include "SphereCollider.h"
#include "ParticleSystem.h"

RuinsScript::RuinsScript() {}

RuinsScript::~RuinsScript() {}

void RuinsScript::LateUpdate() {
	// ���� �����찡 ��Ŀ�� �Ǿ��� ���� �Է� ó��
	if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
	{
		if (INPUT->GetButton(KEY_TYPE::SUBTRACT)) {
			ApplyModuleAbillities();
			GET_SINGLE(SceneManager)->LoadScene(L"FactoryScene");
		}
	}

	Occupation();
}
void RuinsScript::Occupation()
{
	if (!_water && !_player && !_occupationUI)
	{
		if (GET_SINGLE(SceneManager)->FindObjectByName(L"Water")) _water = GET_SINGLE(SceneManager)->FindObjectByName(L"Water");
		if (GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ")) _player = GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ");
		if (GET_SINGLE(SceneManager)->FindObjectByName(L"OccupationUI")) _occupationUI = GET_SINGLE(SceneManager)->FindObjectByName(L"OccupationUI");
	}

	if (_water->GetTransform()->GetLocalPosition().y < -200.f)
	{
		_isClear = true;
		if (!_isCreatePortal)
		{
			_occupationUI->SetRenderOff();
			_isCreatePortal = true;
			CreatePortal();
		}
	}

	if (!_isClear)
	{
		auto playerPos = _player->GetTransform()->GetLocalPosition();

		if (playerPos.x >= -2000.0f && playerPos.x <= 2000.0f &&
			playerPos.z >= -2000.0f && playerPos.z <= 2000.0f)
		{
			Vec3 pos = _water->GetTransform()->GetLocalPosition();
			pos.y -= 0.2f;
			_water->GetTransform()->SetLocalPosition(pos);
			BlinkUI();
		}
		else
		{
			Vec3 pos = _water->GetTransform()->GetLocalPosition();
			pos.y += 0.1f;
			_water->GetTransform()->SetLocalPosition(pos);
			_occupationUI->SetRenderOff();
		}
	}
}

void RuinsScript::BlinkUI()
{
	_elapsedTime += DELTA_TIME;

	if (_elapsedTime >= _blinkTime) // _blinkTime(1��)���� ���� ����
	{
		_isVisible = !_isVisible; // ���� ����
		_elapsedTime = 0.0f; // Ÿ�̸� �ʱ�ȭ
	}

	if (_isVisible)
		_occupationUI->SetRenderOn();
	else
		_occupationUI->SetRenderOff();
}

void RuinsScript::CreatePortal()
{
	auto portalParticle = GET_SINGLE(SceneManager)->FindObjectByName(L"potalParticle");
	portalParticle->GetParticleSystem()->ParticleStart();

	auto portalFrameParticle = GET_SINGLE(SceneManager)->FindObjectByName(L"portalFrameParticle");
	portalFrameParticle->GetParticleSystem()->ParticleStart();
}
