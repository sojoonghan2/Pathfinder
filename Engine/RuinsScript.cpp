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
#include "MessageManager.h"

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

	// SERVER TODO: 이거를 loadingscript로.
#ifdef NETWORK_ENABLE
	auto& queue = GET_SINGLE(MessageManager)->GetMessageQueue(ID_RUIN_SCENE);
	while (not queue.empty()) {
		auto& message{ queue.front() };
		switch (message->type) {
		case MsgType::START_GAME:
		{
			_isStart = true;
			cout << "Ruins Scene Start!\n";
			GET_SINGLE(SceneManager)->FindObjectByName(L"WaitUI")->SetRenderOff();
		}
		break;
		default: break;
		}
		queue.pop();
	}

#endif // NETWORK_ENABLE
	if (INPUT->GetButton(KEY_TYPE::N))
	{
		_isStart = true;
		cout << "Ruins Scene Start!\n";
		GET_SINGLE(SceneManager)->FindObjectByName(L"WaitUI")->SetRenderOff();
	}

	//if(_isStart) Occupation();
}

void RuinsScript::Start()
{
	_waitUI = GET_SINGLE(SceneManager)->FindObjectByName(L"WaitUI");
	_portalParticle = GET_SINGLE(SceneManager)->FindObjectByName(L"potalParticle");
	_portalFrameParticle = GET_SINGLE(SceneManager)->FindObjectByName(L"portalFrameParticle");
	_water = GET_SINGLE(SceneManager)->FindObjectByName(L"Water");
	_player = GET_SINGLE(SceneManager)->FindObjectByName(L"Player");
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
			auto pos = _water->GetTransform()->GetLocalPosition();
			pos.y = -10000.f;
			_water->GetTransform()->SetLocalPosition(pos);
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