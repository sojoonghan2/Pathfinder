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

	// �� �ε� �۾� ����
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

	// �ε� �Ϸ� �÷��� ����
	cout << "Scene loading completed, setting loadEnd flag" << endl;

#ifdef NETWORK_ENABLE
	// ����ó������ ���ָ� ������?
	GET_SINGLE(SocketIO)->DoSend<packet::CSLoadComplete>();
#endif // NETWORK_ENABLE

	loadEnd = true;
}