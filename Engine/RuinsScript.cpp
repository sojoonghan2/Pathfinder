#include "pch.h"
#include "RuinsScript.h"
#include "Input.h"
#include "Engine.h"
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

RuinsScript::RuinsScript() {}

RuinsScript::~RuinsScript() {}

void RuinsScript::LateUpdate() {
	// °ÔÀÓ À©µµ¿ì°¡ Æ÷Ä¿½º µÇ¾úÀ» ¶§¸¸ ÀÔ·Â Ã³¸®
	if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
	{
		if (INPUT->GetButton(KEY_TYPE::SUBTRACT)) {
			ApplyModuleAbillities();
			GET_SINGLE(SceneManager)->LoadScene(L"FactoryScene");
		}
	}

	// SERVER TODO: ÀÌ°Å¸¦ loadingscript·Î.
#ifdef NETWORK_ENABLE
	auto& queue = GET_SINGLE(MessageManager)->GetMessageQueue(ID_RUIN_SCENE);
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
		// ¸ðµâÀ» °ñ¶úÀ¸¸é ´ÙÀ½ ¾ÀÀ¸·Î
		shared_ptr<LoadingScene> loadingScene = make_shared<LoadingScene>();
		GET_SINGLE(SceneManager)->RegisterScene(L"LoadingScene", loadingScene);
		loadingScene->Init(RoomType::Factory);
		GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");
	}
<<<<<<< HEAD

	// Vrs Mode Change
	/*
	if (INPUT->GetButtonDown(KEY_TYPE::K)) {
		GET_SINGLE(SceneManager)->GetActiveScene()->SetRenderingRate(D3D12_VARIABLE_SHADING_RATE_TIER_2);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::L)) {
		GET_SINGLE(SceneManager)->GetActiveScene()->SetRenderingRate(D3D12_VARIABLE_SHADING_RATE_TIER_1);
	}
	*/
	// µð¹ö±× ·»´õ·¯ On/Off
	if (INPUT->GetButtonDown(KEY_TYPE::J))
	{
		// Wire ¿ÀºêÁ§Æ® ºñÈ°¼ºÈ­
		auto wireObjects = GET_SINGLE(SceneManager)->FindObjectsByNameContains(L"Wire");
		for (auto& obj : wireObjects)
		{
			if (obj->IsRender()) obj->SetRender(false);
			else obj->SetRender(true);
		}

		// dummy ¿ÀºêÁ§Æ® ºñÈ°¼ºÈ­
		auto dummyObjects = GET_SINGLE(SceneManager)->FindObjectsByNameContains(L"dummy");
		for (auto& obj : dummyObjects)
		{
			if (obj->IsRender()) obj->SetRender(false);
			else obj->SetRender(true);
		}
	}
=======
>>>>>>> parent of e521ebe (Jë¡œ ì™€ì´ì–´ ê»ë‹¤ì¼°ë‹¤ ê°€ëŠ¥)
}

void RuinsScript::Start()
{
	// ¸®¼Ò½º Ä³½Ì
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
}

void RuinsScript::Occupation()
{
	// Á¡·É Á¶°Ç
	auto playerPos = _player->GetTransform()->GetLocalPosition();
	if (playerPos.x >= -2000.0f && playerPos.x <= 2000.0f &&
		playerPos.z >= -2000.0f && playerPos.z <= 2000.0f)
	{
		Vec3 pos = _water->GetTransform()->GetLocalPosition();
		pos.y -= DELTA_TIME * 7.6667;
		_water->GetTransform()->SetLocalPosition(pos);
		BlinkUI();
	}

	// ¾À Á¾·á Á¶°Ç
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
	// Æ÷Å»¿¡ µé¾î¿Ô´ÂÁö È®ÀÎ
	// ÇÑ ¸íÀÌ¶óµµ Æ÷Å»¿¡ µé¾î°¡¸é Ã³¸®ÇÏ´Â°Ô ÁÁÀ» µí


	auto playerPos = _player->GetTransform()->GetLocalPosition();
	if (playerPos.x >= -100.0f && playerPos.x <= 100.0f &&
		playerPos.z >= 3900.0f && playerPos.z <= 4100.0f)
	{
		cout << "Æ÷Å» µµÂø\n";
		// ÀÏ´Ü Æ÷Å» µµÂøÇÏ¸é ¹Ù·Î ´ÙÀ½ ¾À ÀÌµ¿
		_currentState = StageState::LOAD_NEXT_SCENE;
	}
}

void RuinsScript::ModuleSelect()
{
	/*
	if (¸ðµâÀ» ¼±ÅÃÇÏ¶ó´Â ¸Þ½ÃÁö)
	{
		for (int i = 0; i < 3; ++i) _module[i]µéÀÇ ¸ðµâ ½ºÅ©¸³Æ®->Activate();
	}

	if (¸ðµâÀ» ¼±ÅÃÇß´Ù´Â ¸Þ½ÃÁö)
	{
		_module[selectedIdx]->GetScript<ModuleScript>()->OnSelected();

		_currentState = StageState::MODULE_SELECT;
	}
	*/
}

void RuinsScript::WaitOtherModule()
{
	/*
	if (¸ðµç ÇÃ·¹ÀÌ¾î°¡ ¸ðµâÀ» °ñ¶úÀ¸¸é)
	{
		for (int i = 0; i < 3; ++i) _module[i]µéÀÇ ¸ðµâ ½ºÅ©¸³Æ®->Deactivate();

		_currentState = StageState::LOAD_NEXT_SCENE;
	}
	*/
}