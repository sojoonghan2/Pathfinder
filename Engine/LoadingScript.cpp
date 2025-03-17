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
    // ���⼭ ������ ����
    StartLoadingThread();
}

LoadingScript::~LoadingScript()
{
    // �����尡 ���� ���� ���̶�� ����
    if (loadThread != nullptr && loadThread->joinable())
    {
        loadThread->join();
        delete loadThread;
        loadThread = nullptr;
    }
}

// ������ ���� �Լ� �и�
void LoadingScript::StartLoadingThread()
{
    if (isInitialized)
        return;

    isInitialized = true;
    loadEnd = false;

    // ����� ���
    cout << "Starting loading thread..." << endl;

    // ���� �����尡 ������ ����
    if (loadThread != nullptr && loadThread->joinable())
    {
        loadThread->join();
        delete loadThread;
    }

    // �� ������ ���� �� ����
    loadThread = new std::thread(&LoadingScript::SceneLoad, this);
}

void LoadingScript::LateUpdate()
{
    // ���� �ʱ�ȭ���� �ʾҴٸ� �ʱ�ȭ
    if (!isInitialized)
    {
        StartLoadingThread();
    }

    if (loadEnd && loadThread != nullptr)
    {
        cout << "Loading completed, preparing to switch scenes..." << endl;

        // �����尡 ���� ���� ���̶�� �Ϸ�� ������ ���
        if (loadThread->joinable())
        {
            loadThread->join();
            delete loadThread;
            loadThread = nullptr;
        }

        // �ε��� �Ϸ�Ǿ����Ƿ� ���� ������ ��ȯ
        cout << "Switching to RuinsScene..." << endl;
        GET_SINGLE(SceneManager)->LoadScene(L"RuinsScene");
    }
}

void LoadingScript::SceneLoad()
{
    cout << "SceneLoad thread started" << endl;

    // �� �ε� �۾� ����
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

    // �ε� �Ϸ� �÷��� ����
    cout << "Scene loading completed, setting loadEnd flag" << endl;
    loadEnd = true;
}