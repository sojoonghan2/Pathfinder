#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"

#include "TestCameraScript.h"
#include "PlayerScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "SphereCollider.h"
#include "MeshData.h"
#include "TestDragon.h"

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

void SceneManager::Init()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion
#pragma region AddScene
	shared_ptr<TitleScene> titleScene = make_shared<TitleScene>();
	scenes[L"TitleScene"] = titleScene->GetScene();

#if defined(ALLLOAD)
	shared_ptr<LoadingScene> loadingScene = make_shared<LoadingScene>();
	scenes[L"LoadingScene"] = loadingScene->GetScene();
#endif

#if defined(RUINSLOAD)
	shared_ptr<RuinsScene> ruinsScene = make_shared<RuinsScene>();
	scenes[L"RuinsScene"] = ruinsScene->GetScene();
#endif

#if defined(FACTORYLOAD)
	shared_ptr<FactoryScene> factoryScene = make_shared<FactoryScene>();
	scenes[L"FactoryScene"] = factoryScene->GetScene();
#endif

#if defined(EXPLORATIONLOAD)
	shared_ptr<ExplorationScene> explorationScene = make_shared<ExplorationScene>();
	scenes[L"ExplorationScene"] = explorationScene->GetScene();
#endif

#if defined(CRASHLOAD)
	shared_ptr<CrashScene> crashScene = make_shared<CrashScene>();
	scenes[L"CrashScene"] = crashScene->GetScene();
#endif

#if defined(LUCKYLOAD)
	shared_ptr<LuckyScene> luckyScene = make_shared<LuckyScene>();
	scenes[L"LuckyScene"] = luckyScene->GetScene();
#endif

#if defined(BOSSLOAD)
	shared_ptr<BossScene> bossScene = make_shared<BossScene>();
	scenes[L"BossScene"] = bossScene->GetScene();
#endif

#if defined(TESTLOAD)
	shared_ptr<TestScene> testScene = make_shared<TestScene>();
	scenes[L"TestScene"] = testScene->GetScene();
#endif

#if defined(PARTICLELOAD)
	shared_ptr<ParticleScene> particleScene = make_shared<ParticleScene>();
	scenes[L"ParticleScene"] = particleScene->GetScene();
#endif

#pragma endregion
}

void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->FinalUpdate();
}

void SceneManager::Render()
{
	if (_activeScene) {
		_activeScene->Render();
	}
}

void SceneManager::LoadScene(wstring sceneName)
{
	shared_ptr<Scene> scene = make_shared<Scene>();

	_activeScene = scenes.at(sceneName);

	_activeScene->Awake();
	_activeScene->Start();
}

void SceneManager::ChangeScene(wstring sceneName) {
	_activeScene = scenes.at(sceneName);
}

void SceneManager::RegisterScene(const wstring& name, shared_ptr<Scene> scene)
{
	scenes[name] = scene;
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// 기존 데이터 삭제
	const wstring& prevName = _layerNames[index];
	_layerIndex.erase(prevName);

	_layerNames[index] = name;
	_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = _layerIndex.find(name);
	if (findIt == _layerIndex.end())
		return 0;

	return findIt->second;
}

shared_ptr<GameObject> SceneManager::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetActiveScene()->GetMainCamera();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// ViewSpace에서 Picking 진행
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCollider() == nullptr)
			continue;

		// ViewSpace에서의 Ray 정의
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace에서의 Ray 정의
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace에서 연산
		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
}

shared_ptr<GameObject> SceneManager::FindObjectByName(const wstring& name)
{
	if (_activeScene == nullptr)
		return nullptr;

	const vector<shared_ptr<GameObject>>& objects = _activeScene->GetGameObjects();

	for (const auto& obj : objects)
	{
		if (obj->GetName() == name)
		{
			return obj; // 이름이 일치하는 오브젝트를 찾으면 반환
		}
	}

	return nullptr; // 찾지 못하면 nullptr 반환
}