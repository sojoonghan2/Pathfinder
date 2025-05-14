#include "pch.h"
#include "RuinsScript.h"
#include "Input.h"
#include "Application.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "GameModule.h"
#include "Timer.h"
#include "SphereCollider.h"
#include "ParticleSystem.h"
#include "MessageManager.h"
#include "LoadingScene.h"
#include "ModuleScript.h"
#include "MeshRenderer.h"

RuinsScript::RuinsScript() {}

RuinsScript::~RuinsScript() {}

void RuinsScript::LateUpdate()
{
	// SERVER TODO: 이거를 loadingscript로.
#ifdef NETWORK_ENABLE
	auto& queue = GET_SINGLE(MessageManager)->GetMessageQueue(_id);
	while (not queue.empty()) {
		auto& message{ queue.front() };
		switch (message->type) {
		case MsgType::START_GAME:
		{
			_currentState = StageState::PLAYING;
			cout << "Ruins Scene Start!\n";
			_waitUI->SetRender(false);
		}
		break;
		default: break;
		}
		queue.pop();
	}

#endif // NETWORK_ENABLE
	if (INPUT->GetButton(KEY_TYPE::N) && (_currentState == StageState::START))
	{
		_currentState = StageState::PLAYING;
		cout << "Ruins Scene Start!\n";
		_waitUI->SetRender(false);
	}

	if(_currentState == StageState::PLAYING) Occupation();
	if (_currentState == StageState::PORTAL_CREATE) IsPortalZone();
	if (_currentState == StageState::PORTAL_ENTERED) ModuleSelect();
	if (_currentState == StageState::MODULE_SELECT) WaitOtherModule();
	if (_currentState == StageState::LOAD_NEXT_SCENE)
	{
		// 모듈을 골랐으면 다음 씬으로
		shared_ptr<LoadingScene> loadingScene = make_shared<LoadingScene>();
		GET_SINGLE(SceneManager)->RegisterScene(L"LoadingScene", loadingScene);
		loadingScene->Init(RoomType::FACTORY);
		GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");
	}

	// 디버그 렌더러 On/Off
	if (INPUT->GetButtonDown(KEY_TYPE::J))
	{
		// Wire 오브젝트 비활성화
		auto wireObjects = GET_SINGLE(SceneManager)->FindObjectsByNameContains(L"Wire");
		for (auto& obj : wireObjects)
		{
			if (obj->IsRender()) obj->SetRender(false);
			else obj->SetRender(true);
		}

		// dummy 오브젝트 비활성화
		auto dummyObjects = GET_SINGLE(SceneManager)->FindObjectsByNameContains(L"dummy");
		for (auto& obj : dummyObjects)
		{
			if (obj->IsRender()) obj->SetRender(false);
			else obj->SetRender(true);
		}
	}
}

void RuinsScript::Awake()
{
	GET_SINGLE(MessageManager)->RegisterStaticObject(ID_SCENE_RUINS, _id);
}

void RuinsScript::Start()
{
	// 리소스 캐싱
	_waitUI = GET_SINGLE(SceneManager)->FindObjectByName(L"WaitUI");
	_portalParticle = GET_SINGLE(SceneManager)->FindObjectByName(L"potalParticle");
	_portalFrameParticle = GET_SINGLE(SceneManager)->FindObjectByName(L"portalFrameParticle");
	_water = GET_SINGLE(SceneManager)->FindObjectByName(L"Water");
	_player = GET_SINGLE(SceneManager)->FindObjectByName(L"Player");
	_occupationUI = GET_SINGLE(SceneManager)->FindObjectByName(L"OccupationUI");

	for (int i = 0; i < 3; ++i)
	{
		std::wstring name = L"RuinsModule" + std::to_wstring(i);
		shared_ptr<GameObject> go = GET_SINGLE(SceneManager)->FindObjectByName(name);
		_module[i] = dynamic_pointer_cast<GameModule>(go);
	}
	INPUT->LockCursor(true);

	auto pos = _player->GetTransform()->GetLocalPosition();
	pos.y = -100.f;
	_player->GetTransform()->SetLocalPosition(pos);
}


void RuinsScript::Occupation()
{
	// 점령 조건
	auto playerPos = _player->GetTransform()->GetLocalPosition();
	if (playerPos.x >= -2000.0f && playerPos.x <= 2000.0f &&
		playerPos.z >= -2000.0f && playerPos.z <= 2000.0f)
	{
		Vec3 pos = _water->GetTransform()->GetLocalPosition();
		pos.y -= DELTA_TIME * 7.6667;
		_water->GetTransform()->SetLocalPosition(pos);
		BlinkUI();
	}

	// 씬 종료 조건
	if (_water->GetTransform()->GetLocalPosition().y < -130.f)
	{
		_occupationUI->SetRender(false);
		auto pos = _water->GetTransform()->GetLocalPosition();
		pos.y = -10000.f;
		_water->GetTransform()->SetLocalPosition(pos);
		_water->SetRender(false);
		CreatePortal();
	}
}

void RuinsScript::BlinkUI()
{
	_elapsedTime += DELTA_TIME;

	if (_elapsedTime >= _blinkTime)
	{
		_isVisible = !_isVisible;
		_elapsedTime = 0.0f;
	}

	if (_isVisible)
		_occupationUI->SetRender(true);
	else
		_occupationUI->SetRender(false);
}

void RuinsScript::CreatePortal()
{
	if (_portalParticle)
		_portalParticle->GetParticleSystem()->ParticleStart();
	if (_portalFrameParticle)
		_portalFrameParticle->GetParticleSystem()->ParticleStart();

	_currentState = StageState::PORTAL_CREATE;
}

void RuinsScript::IsPortalZone()
{
	// 포탈에 들어왔는지 확인
	// 한 명이라도 포탈에 들어가면 처리하는게 좋을 듯


	auto playerPos = _player->GetTransform()->GetLocalPosition();
	if (playerPos.x >= -100.0f && playerPos.x <= 100.0f &&
		playerPos.z >= 3900.0f && playerPos.z <= 4100.0f)
	{
		cout << "포탈 도착\n";
		// 일단 포탈 도착하면 바로 다음 씬 이동
		_currentState = StageState::LOAD_NEXT_SCENE;
	}
}

void RuinsScript::ModuleSelect()
{
	/*
	if (모듈을 선택하라는 메시지)
	{
		for (int i = 0; i < 3; ++i) _module[i]->GetScript<ModuleScript>()->Activate();
	}

	if (모듈을 선택했다는 메시지)
	{
		_module[selectedIdx]->GetScript<ModuleScript>()->OnSelected();

		_currentState = StageState::MODULE_SELECT;
	}
	*/
}

void RuinsScript::WaitOtherModule()
{
	/*
	if (모든 플레이어가 모듈을 골랐다는 메시지)
	{
		for (int i = 0; i < 3; ++i) _module[i]->GetScript<ModuleScript>()->Deactivate();

		_currentState = StageState::LOAD_NEXT_SCENE;
	}
	*/
}