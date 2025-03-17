#include "pch.h"
#include "LoadingScript.h"
#include "Input.h"
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
{
    // 여기서 스레드 시작
    StartLoadingThread();
}

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
    // 아직 초기화되지 않았다면 초기화
    if (!isInitialized)
    {
        StartLoadingThread();
    }

    if (loadEnd && loadThread != nullptr)
    {
        cout << "Loading completed, preparing to switch scenes..." << endl;

        // 스레드가 아직 실행 중이라면 완료될 때까지 대기
        if (loadThread->joinable())
        {
            loadThread->join();
            delete loadThread;
            loadThread = nullptr;
        }

        // 로딩이 완료되었으므로 다음 씬으로 전환
        cout << "Switching to RuinsScene..." << endl;
        GET_SINGLE(SceneManager)->LoadScene(L"RuinsScene");
    }
}

void LoadingScript::SceneLoad()
{
    cout << "SceneLoad thread started" << endl;

    // 씬 로딩 작업 수행
    {
        cout << "Loading TitleScene..." << endl;
        shared_ptr<TitleScene> titleScene = make_shared<TitleScene>();
        GET_SINGLE(SceneManager)->RegisterScene(L"TitleScene", titleScene->GetScene());

        cout << "Loading RuinsScene..." << endl;
        shared_ptr<RuinsScene> ruinsScene = make_shared<RuinsScene>();
        GET_SINGLE(SceneManager)->RegisterScene(L"RuinsScene", ruinsScene->GetScene());

        cout << "Loading FactoryScene..." << endl;
        shared_ptr<FactoryScene> factoryScene = make_shared<FactoryScene>();
        GET_SINGLE(SceneManager)->RegisterScene(L"FactoryScene", factoryScene->GetScene());

        cout << "Loading ExplorationScene..." << endl;
        shared_ptr<ExplorationScene> explorationScene = make_shared<ExplorationScene>();
        GET_SINGLE(SceneManager)->RegisterScene(L"ExplorationScene", explorationScene->GetScene());

        cout << "Loading CrashScene..." << endl;
        shared_ptr<CrashScene> crashScene = make_shared<CrashScene>();
        GET_SINGLE(SceneManager)->RegisterScene(L"CrashScene", crashScene->GetScene());

        cout << "Loading LuckyScene..." << endl;
        shared_ptr<LuckyScene> luckyScene = make_shared<LuckyScene>();
        GET_SINGLE(SceneManager)->RegisterScene(L"LuckyScene", luckyScene->GetScene());

        cout << "Loading BossScene..." << endl;
        shared_ptr<BossScene> bossScene = make_shared<BossScene>();
        GET_SINGLE(SceneManager)->RegisterScene(L"BossScene", bossScene->GetScene());
    }

    // 로딩 완료 플래그 설정
    cout << "Scene loading completed, setting loadEnd flag" << endl;
    loadEnd = true;
}