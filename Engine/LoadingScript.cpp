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

LoadingScript::LoadingScript() : loadEnd(false), loadThread(nullptr), isInitialized(false)
{}

LoadingScript::~LoadingScript()
{
	// 스레드가 아직 실행 중이라면 정리
	if (loadThread != nullptr && loadThread->joinable())
	{
		loadThread->join();
		delete loadThread;
		loadThread = nullptr;
	}
}

// 스레드 시작 함수 분리
void LoadingScript::StartLoadingThread()
{
	if (isInitialized)
		return;

	isInitialized = true;
	loadEnd = false;

	// 디버그 출력
	cout << "Starting loading thread..." << endl;

	// 이전 스레드가 있으면 정리
	if (loadThread != nullptr && loadThread->joinable())
	{
		loadThread->join();
		delete loadThread;
	}

	// 새 스레드 생성 및 시작
#ifdef NETWORK_ENABLE
	roomType = GET_SINGLE(SocketIO)->GetRoomType();
#endif // NETWORK_ENABLE

#ifndef NETWORK_ENABLE
	roomType = RoomType::Ruin;
#endif // !NETWORK_ENABLE

	loadThread = new std::thread(&LoadingScript::SceneLoad, this);
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

	// 아직 초기화되지 않았다면 초기화
	if (!isInitialized)
	{
		StartLoadingThread();
	}

	if (loadEnd && loadThread != nullptr)
	{
		cout << "Loading completed, preparing to switch scenes..." << endl;

		if (loadThread->joinable())
		{
			loadThread->join();
			delete loadThread;
			loadThread = nullptr;
		}

		cout << "Switching to RuinsScene..." << endl;
		GET_SINGLE(SceneManager)->LoadScene(L"RuinsScene");
	}
}

void LoadingScript::Awake()
{
	// SERVER TODO:
	// 메시지로 로딩할 씬 받아와서 로드

	auto icon = GET_SINGLE(SceneManager)->FindObjectByName(L"LoadingIcon");
	if (icon)
	{
		icon->SetRender(true);
	}

	StartLoadingThread();
}

void LoadingScript::SceneLoad()
{
	cout << "SceneLoad thread started" << endl;

	// 씬 로딩 작업 수행
	{
		cout << "Loading RuinsScene..." << endl;
		switch (roomType)
		{
		case RoomType::None:
		{
			cout << "ERROR: Type is not exist" << endl;
		}
		break;
		case RoomType::Ruin:
		{
			shared_ptr<RuinsScene> ruinsScene = make_shared<RuinsScene>();
			GET_SINGLE(SceneManager)->RegisterScene(L"RuinsScene", ruinsScene->GetScene());
			ruinsScene->Init();
		}
		break;
		case RoomType::Factory:
		{
			shared_ptr<FactoryScene> factoryScene = make_shared<FactoryScene>();
			GET_SINGLE(SceneManager)->RegisterScene(L"FactoryScene", factoryScene->GetScene());
		}
		break;
		case RoomType::Ristrict:
		{
		}	
		break;
		case RoomType::Falling:
		{
		}
		break;
		case RoomType::Lucky:
		{
		}
		break;
		default:
			break;
		}
	}

	// 로딩 완료 플래그 설정
	cout << "Scene loading completed, setting loadEnd flag" << endl;

#ifdef NETWORK_ENABLE
	// 예외처리정도 해주면 좋을듯?
	GET_SINGLE(SocketIO)->DoSend<packet::CSLoadComplete>();
#endif // NETWORK_ENABLE

	loadEnd = true;
}