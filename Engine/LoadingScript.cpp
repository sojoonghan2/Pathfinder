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

LoadingScript::LoadingScript() : loadEnd(false), loadThread(nullptr), isInitialized(false), pendingThreadJoin(false) {}

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
#else
	roomType = RoomType::Ruin;
#endif

	loadThread = new std::thread(&LoadingScript::SceneLoad, this);
}

void LoadingScript::SceneLoad()
{
	cout << "SceneLoad thread started" << endl;

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
		cout << "ERROR: Unhandled RoomType" << endl;
		break;
	}

#ifdef NETWORK_ENABLE
	GET_SINGLE(SocketIO)->DoSend<packet::CSLoadComplete>();
#endif

	loadEnd = true;
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

	// 로딩 완료 후 Init, LoadScene
	if (loadEnd && _sceneToInit && !_sceneNameToLoad.empty())
	{
		cout << "Loading complete..." << endl;

		_sceneToInit->Init(); // 메인 스레드에서 안전하게 Init
		GET_SINGLE(SceneManager)->LoadScene(_sceneNameToLoad);

		_sceneToInit = nullptr;
		_sceneNameToLoad.clear();
		pendingThreadJoin = true;
	}

	// join은 Init 이후 한 프레임 뒤에 실행
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