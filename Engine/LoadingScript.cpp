#include "pch.h"
#include "LoadingScript.h"
#include "Input.h"
#include "Timer.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "LoadingScene.h"
#include "TitleScene.h"
#include "ParticleScene.h"
#include "RuinsScene.h"
#include "FactoryScene.h"
#include "ExplorationScene.h"
#include "CrashScene.h"
#include "LuckyScene.h"
#include "BossScene.h"
#include "TestScene.h"
#include "SocketIO.h"

LoadingScript::LoadingScript(RoomType type)
	: loadEnd(false), loadThread(nullptr), isInitialized(false), pendingThreadJoin(false), _state(LoadingState::Idle), roomType(type)
{
}

LoadingScript::~LoadingScript()
{
	if (loadThread && loadThread->joinable())
	{
		loadThread->join();
		delete loadThread;
		loadThread = nullptr;
	}
}

void LoadingScript::Awake()
{
	auto icon = GET_SINGLE(SceneManager)->FindObjectByName(L"LoadingIcon");
	if (icon)
		icon->SetRender(true);

	StartLoadingThread();
}

void LoadingScript::StartLoadingThread()
{
	if (isInitialized)
		return;

	isInitialized = true;
	loadEnd = false;

	if (loadThread && loadThread->joinable())
	{
		loadThread->join();
		delete loadThread;
	}

#ifdef NETWORK_ENABLE
	roomType = GET_SINGLE(SocketIO)->GetRoomType();
#endif

	loadThread = new std::thread(&LoadingScript::SceneLoad, this);
}

void LoadingScript::SceneLoad()
{
	cout << "SceneLoad thread started\n";

	switch (roomType)
	{
	case RoomType::Ruin:
	{
		auto scene = make_shared<RuinsScene>();
		GET_SINGLE(SceneManager)->RegisterScene(L"RuinsScene", scene);
		_sceneToInit = scene;
		_sceneNameToLoad = L"RuinsScene";
	}
	break;

	case RoomType::Factory:
	{
		auto scene = make_shared<FactoryScene>();
		GET_SINGLE(SceneManager)->RegisterScene(L"FactoryScene", scene);
		_sceneToInit = scene;
		_sceneNameToLoad = L"FactoryScene";
	}
	break;

	case RoomType::Lucky:
	{
		auto scene = make_shared<LuckyScene>();
		GET_SINGLE(SceneManager)->RegisterScene(L"LuckyScene", scene);
		_sceneToInit = scene;
		_sceneNameToLoad = L"LuckyScene";
	}
	break;

	default:
		cout << "ERROR: Unhandled RoomType\n";
		break;
	}

#ifdef NETWORK_ENABLE
	GET_SINGLE(SocketIO)->DoSend<packet::CSLoadComplete>();
#endif

	loadEnd = true;

	_state = LoadingState::ReadyToInit;
}

void LoadingScript::LateUpdate()
{
	// 아이콘 회전
	auto icon = GET_SINGLE(SceneManager)->FindObjectByName(L"LoadingIcon");
	if (icon)
	{
		Vec3 rotation = icon->GetTransform()->GetLocalRotation();
		rotation.z += DELTA_TIME * 3.f;
		icon->GetTransform()->SetLocalRotation(rotation);
	}

	if (!isInitialized)
		StartLoadingThread();

	if (_state == LoadingState::ReadyToInit)
	{
		cout << "ReadyToInit\n";
		_sceneToInit->Init();
		_state = LoadingState::ReadyToLoad;
		return;
	}

	if (_state == LoadingState::ReadyToLoad)
	{
		cout << "ReadyToLoad\n";
		GET_SINGLE(SceneManager)->LoadScene(_sceneNameToLoad);
		_sceneToInit = nullptr;
		_sceneNameToLoad.clear();
		_state = LoadingState::Switching;
		pendingThreadJoin = true;
	}

	// 백그라운드 스레드 정리
	if (pendingThreadJoin)
	{
		if (loadThread)
		{
			try
			{
				if (loadThread->joinable())
					loadThread->join();
			}
			catch (const std::exception& e)
			{
				cout << "Thread join failed: " << e.what() << endl;
			}
			delete loadThread;
			loadThread = nullptr;
		}
		pendingThreadJoin = false;
	}
}