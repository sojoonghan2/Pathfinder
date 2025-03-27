#include "pch.h"
#include "LuckyScene.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Shader.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "ParticleSystem.h"
#include "TestCameraScript.h"
#include "Resources.h"
#include "MeshData.h"

#include "TestDragon.h"
#include "PlayerScript.h"
#include "WaterScript.h"
#include "LuckyScript.h"
#include "ModuleScript.h"

#include "SphereCollider.h"
#include "GameModule.h"

#include "GlitterParticleSystem.h"
#include "TestParticleScript.h"

LuckyScene::LuckyScene()
{
// 컴퓨트 셰이더, 멀티쓰레드로 작업이 가능
#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV 용 Texture 생성
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// 쓰레드 그룹 (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

// 카메라
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, FOV=45도
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->AddComponent(make_shared<LuckyScript>());
		camera->GetCamera()->SetFar(100000.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
		activeScene->AddGameObject(camera);
	}
#pragma endregion

// UI 카메라
#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
		activeScene->AddGameObject(camera);
	}
#pragma endregion

// 스카이 박스
#pragma region SkyBox
	{
		shared_ptr<GameObject> skybox = make_shared<GameObject>();
		skybox->AddComponent(make_shared<Transform>());
		skybox->SetCheckFrustum(true);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Sky01", L"..\\Resources\\Texture\\Sky01.jpg");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);
		activeScene->AddGameObject(skybox);
	}
#pragma endregion

// 터레인 큐브
#pragma region TerrainCube
	{
		// 1. 기본 오브젝트 생성 및 설정
		shared_ptr<GameObject> terraincube = make_shared<GameObject>();
		terraincube->AddComponent(make_shared<Transform>());
		terraincube->SetCheckFrustum(true);

		// 2. Transform 컴포넌트 추가 및 설정
		terraincube->AddComponent(make_shared<Transform>());
		// 씬의 임시 크기
		terraincube->GetTransform()->SetLocalScale(Vec3(10000.f, 10000.f, 10000.f));
		// 씬의 임시 좌표
		terraincube->GetTransform()->SetLocalPosition(Vec3(0.f, 4900.f, 0.f));

		// 3. MeshRenderer 설정
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TerrainCube");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Lucky", L"..\\Resources\\Texture\\TerrainCube\\Lucky.jpg");
			shared_ptr<Texture> floorTexture = GET_SINGLE(Resources)->Load<Texture>(L"LuckyFloor", L"..\\Resources\\Texture\\TerrainCube\\LuckyFloor.jpg");
			shared_ptr<Texture> topTexture = GET_SINGLE(Resources)->Load<Texture>(L"LuckyTop", L"..\\Resources\\Texture\\TerrainCube\\LuckyTop.jpg");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetTexture(1, floorTexture);
			material->SetTexture(2, topTexture);
			meshRenderer->SetMaterial(material);
		}
		terraincube->AddComponent(meshRenderer);

		// 4. Scene에 추가
		activeScene->AddGameObject(terraincube);
	}
#pragma endregion

// 플레이어
#pragma region Player
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Player\\Player.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		gameObjects[0]->SetName(L"OBJ");
		gameObjects[0]->SetCheckFrustum(false);
		gameObjects[0]->AddComponent(make_shared<TestDragon>());
		gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(0.0f, -500.0f, -500.0f));
		gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(-1.5708f, 3.1416f, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
		gameObjects[0]->AddComponent(make_shared<PlayerScript>());

		activeScene->AddGameObject(gameObjects[0]);
	}
#pragma endregion

// 전역 조명
#pragma region Directional Light
	{
		// 1. light 오브젝트 생성 
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Directional_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

		// 2-1. light 컴포넌트 추가 및 속성 설정
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);

		// 2-2. DIRECTIONAL_LIGHT의 경우 조명 방향 설정
		light->GetLight()->SetLightDirection(Vec3(0.f, 0.f, 0.f));

		// 3. 조명 색상 및 강도 설정
		light->GetLight()->SetDiffuse(Vec3(0.8f, 0.8f, 0.8f));
		light->GetLight()->SetAmbient(Vec3(0.8f, 0.8f, 0.8f));
		light->GetLight()->SetSpecular(Vec3(0.05f, 0.05f, 0.05f));

		// 4. Scene에 추가
		activeScene->AddGameObject(light);
	}
#pragma endregion

// 보물 상자
#pragma region TREASUREBOX
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\treasure_chest\\treasure_chest.fbx");

		vector<shared_ptr<GameObject>> treasureBox = meshData->Instantiate();

		int index = 0;
		for (auto& gameObject : treasureBox)
		{
			// 이름 설정
			gameObject->SetName(L"TreasureChest" + std::to_wstring(index + 1));

			gameObject->SetCheckFrustum(false);

			gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, 6000.f, 0.f));
			shared_ptr<GlitterParticleSystem> glitterParticleSystem = make_shared<GlitterParticleSystem>();
			gameObject->AddComponent(glitterParticleSystem);

			gameObject->GetTransform()->SetLocalScale(Vec3(20.f, 20.f, 20.f));
			gameObject->GetTransform()->SetLocalRotation(Vec3(-1.5708f, 0.0f, 0.0f));
			if (index != 1) gameObject->SetRenderOff();
			activeScene->AddGameObject(gameObject);
			index++;
		}
	}
#pragma endregion

// 모듈
#pragma region Module
	for (int i{}; i < 3; ++i)
	{
		shared_ptr<GameModule> obj = make_shared<GameModule>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->AddComponent(make_shared<ModuleScript>());
		obj->GetTransform()->SetLocalScale(Vec3(300.f, 500.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-400.f + (i * 400), -800.f, 1.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			texture = obj->GetTexture();

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		activeScene->AddGameObject(obj);
	}
#pragma endregion
}

LuckyScene::~LuckyScene() {}