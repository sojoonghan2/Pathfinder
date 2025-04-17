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
	// ������ ȸ��
	auto icon = GET_SINGLE(SceneManager)->FindObjectByName(L"LoadingIcon");
	if (icon)
	{
		Vec3 rotation = icon->GetTransform()->GetLocalRotation();
		rotation.z += DELTA_TIME * 3.f;
		icon->GetTransform()->SetLocalRotation(rotation);
	}

	// ���� �ʱ�ȭ���� �ʾҴٸ� �ʱ�ȭ
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
	// �޽����� �ε��� �� �޾ƿͼ� �ε�
	StartLoadingThread();
}

void LoadingScript::SceneLoad()
{
	cout << "SceneLoad thread started" << endl;

	// �� �ε� �۾� ����
	{
		cout << "Loading RuinsScene..." << endl;
		shared_ptr<RuinsScene> ruinsScene = make_shared<RuinsScene>();
		GET_SINGLE(SceneManager)->RegisterScene(L"RuinsScene", ruinsScene->GetScene());
	}

	// �ε� �Ϸ� �÷��� ����
	cout << "Scene loading completed, setting loadEnd flag" << endl;
	loadEnd = true;
}