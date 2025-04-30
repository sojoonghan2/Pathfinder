#include "pch.h"
#include "ExplorationScene.h"
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
#include "CameraScript.h"
#include "Resources.h"
#include "MeshData.h"

#include "TestDragon.h"
#include "PlayerScript.h"
#include "WaterScript.h"
#include "ExplorationScript.h"
#include "SelfDestructionRobotScript.h"

#include "SphereCollider.h"

ExplorationScene::ExplorationScene() {}

ExplorationScene::~ExplorationScene() {}

void ExplorationScene::Init()
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
		camera->AddComponent(make_shared<CameraScript>());
		camera->AddComponent(make_shared<ExplorationScript>());
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

	// 플레이어
#pragma region Player
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Player\\Player.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		gameObjects[0]->SetName(L"Player");
		gameObjects[0]->SetCheckFrustum(false);
		gameObjects[0]->AddComponent(make_shared<TestDragon>());
		gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(0.0f, -500.0f, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(-PI / 2, PI, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
		gameObjects[0]->AddComponent(make_shared<PlayerScript>());

		activeScene->AddGameObject(gameObjects[0]);

		// 얘가 없으면 조명 버그남
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetName(L"Player");
		obj->SetCheckFrustum(true);
		obj->SetStatic(false);

		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetParent(gameObjects[0]->GetTransform());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
			meshRenderer->SetMaterial(material->Clone());
		}
		obj->AddComponent(meshRenderer);
		obj->SetRender(false);
		activeScene->AddGameObject(obj);

		// 1. light 오브젝트 생성
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Point_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetParent(gameObjects[0]->GetTransform());

		// 2-1. light 컴포넌트 추가 및 속성 설정
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::POINT_LIGHT);

		// 2-2. 점광원 특수 설정
		light->GetLight()->SetLightRange(10500.f);

		// 3. 조명 색상 및 강도 설정 - 모든 값을 낮춤
		light->GetLight()->SetDiffuse(Vec3(0.7f, 0.7f, 0.7f));
		light->GetLight()->SetAmbient(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetSpecular(Vec3(1.0f, 1.0f, 1.0f));

		// 4. Scene에 추가
		activeScene->AddGameObject(light);
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
		terraincube->GetTransform()->SetLocalPosition(Vec3(0, 4900.f, 0.f));

		// 3. MeshRenderer 설정
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TerrainCube");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Exploration", L"..\\Resources\\Texture\\TerrainCube\\Exploration.jpg");
			shared_ptr<Texture> floorTexture = GET_SINGLE(Resources)->Load<Texture>(L"ExplorationFloor", L"..\\Resources\\Texture\\TerrainCube\\ExplorationFloor.jpg");
			shared_ptr<Texture> topTexture = GET_SINGLE(Resources)->Load<Texture>(L"ExplorationTop", L"..\\Resources\\Texture\\TerrainCube\\ExplorationTop.jpg");

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

	// *********************************************
	// UI 테스트
	// 0. 월드 공간 위치 정보
	// 1. 노말 벡터 정보
	// 2. 색상 정보
	// 3. 분산광 결과
	// 4. 반사광 결과
	// 5. 그림자
	// *********************************************
#pragma region UI_Test
	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
	obj->AddComponent(make_shared<Transform>());
	obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (120), 250.f, 500.f));
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

		shared_ptr<Texture> texture;
		texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(2);
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	obj->AddComponent(meshRenderer);
	activeScene->AddGameObject(obj);
#pragma endregion

	// 전역 조명(삭제 예정)
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
		//activeScene->AddGameObject(light);
	}
#pragma endregion

	// 로봇
#pragma region SELFDESTRUCTIONROBOT
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> disX(-4000.0f, 4000.0f);
		std::uniform_real_distribution<float> disZ(-4000.0f, 4000.0f);

		vector<Vec3> positions;
		float minDistance = 300.0f;

		for (int i = 0; i < 20; ++i)
		{
			shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\SelfDestructionRobot\\SelfDestructionRobot.fbx");
			vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

			// 겹치지 않는 랜덤 위치 생성
			Vec3 randomPos;
			bool validPosition = false;
			int maxAttempts = 100; // 무한 루프 방지

			while (!validPosition && maxAttempts > 0)
			{
				randomPos = Vec3(disX(gen), -100.0f, disZ(gen));
				validPosition = true;

				for (const Vec3& pos : positions)
				{
					if ((randomPos - pos).Length() < minDistance)
					{
						validPosition = false;
						break;
					}
				}
				maxAttempts--;
			}

			positions.push_back(randomPos);

			gameObjects[0]->SetName(L"SelfDestructionRobot" + std::to_wstring(i + 1));
			gameObjects[0]->SetCheckFrustum(true);
			gameObjects[0]->GetTransform()->SetLocalPosition(randomPos); // 랜덤 위치 적용
			gameObjects[0]->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
			gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(-1.7f, 0.f, 0.f));
			gameObjects[0]->AddComponent(make_shared<SelfDestructionRobotScript>());
			activeScene->AddGameObject(gameObjects[0]);
		}
	}
#pragma endregion
}