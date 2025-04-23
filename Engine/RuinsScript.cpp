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
	// 게임 윈도우가 포커스 되었을 때만 입력 처리
	if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
	{
		if (INPUT->GetButton(KEY_TYPE::SUBTRACT)) {
			ApplyModuleAbillities();
			GET_SINGLE(SceneManager)->LoadScene(L"FactoryScene");
		}
	}

	// SERVER TODO: 키보드 대신 모든 플레이어 로딩 되면 게임 시작되도록
	if (INPUT->GetButton(KEY_TYPE::N))
	{
		_isStart = true;
		cout << "Ruins Scene Start!\n";
		GET_SINGLE(SceneManager)->FindObjectByName(L"WaitUI")->SetRenderOff();
	}

	if(_isStart) Occupation();
}

void RuinsScript::Start()
{
	_waitUI = GET_SINGLE(SceneManager)->FindObjectByName(L"WaitUI");
	_portalParticle = GET_SINGLE(SceneManager)->FindObjectByName(L"potalParticle");
	_portalFrameParticle = GET_SINGLE(SceneManager)->FindObjectByName(L"portalFrameParticle");
	_water = GET_SINGLE(SceneManager)->FindObjectByName(L"Water");
	_player = GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ");
	_occupationUI = GET_SINGLE(SceneManager)->FindObjectByName(L"OccupationUI");
}

void RuinsScript::Occupation()
{
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

	if (_elapsedTime >= _blinkTime) // _blinkTime(1초)마다 상태 변경
	{
		_isVisible = !_isVisible; // 상태 반전
		_elapsedTime = 0.0f; // 타이머 초기화
	}

	if (_isVisible)
		_occupationUI->SetRenderOn();
	else
		_occupationUI->SetRenderOff();
}

void RuinsScript::CreatePortal()
{
	if (_portalParticle)
		_portalParticle->GetParticleSystem()->ParticleStart();
	if (_portalFrameParticle)
		_portalFrameParticle->GetParticleSystem()->ParticleStart();
}