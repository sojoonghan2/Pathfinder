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
	StartLoadingThread();
}

void LoadingScript::SceneLoad()
{
	cout << "SceneLoad thread started" << endl;

	// 씬 로딩 작업 수행
	{
		cout << "Loading RuinsScene..." << endl;
		shared_ptr<RuinsScene> ruinsScene = make_shared<RuinsScene>();
		GET_SINGLE(SceneManager)->RegisterScene(L"RuinsScene", ruinsScene->GetScene());
	}

	// 로딩 완료 플래그 설정
	cout << "Scene loading completed, setting loadEnd flag" << endl;
	loadEnd = true;
}