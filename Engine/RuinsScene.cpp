#include "pch.h"
#include "RuinsScene.h"
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
#include "OccupationScript.h"
#include "RuinsScript.h"
#include "CrapScript.h"

#include "SphereCollider.h"

#include "DustParticleSystem.h"
#include "GlitterParticleSystem.h"
#include "TestParticleScript.h"
#include "LightPillarParticleSystem.h"

RuinsScene::RuinsScene()
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
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		activeScene->AddGameObject(obj);
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
		camera->AddComponent(make_shared<RuinsScript>());
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

		gameObjects[0]->SetName(L"OBJ");
		gameObjects[0]->SetCheckFrustum(false);
		gameObjects[0]->AddComponent(make_shared<TestDragon>());
		gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(0.0f, -500.0f, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(-1.5708f, 3.1416f, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
		gameObjects[0]->AddComponent(make_shared<PlayerScript>());

		activeScene->AddGameObject(gameObjects[0]);
	}
#pragma endregion


// 터레인 큐브
#pragma region TerrainCube
	{
		// 1. 기본 오브젝트 생성 및 설정
		shared_ptr<GameObject> terraincube = make_shared<GameObject>();
		terraincube->AddComponent(make_shared<Transform>());
		terraincube->SetCheckFrustum(false);
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
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Ruins", L"..\\Resources\\Texture\\TerrainCube\\New_ruins_floor.jpg");
			shared_ptr<Texture> floorTexture = GET_SINGLE(Resources)->Load<Texture>(L"RuinsFloor", L"..\\Resources\\Texture\\TerrainCube\\New_ruins_floor.jpg");
			shared_ptr<Texture> topTexture = GET_SINGLE(Resources)->Load<Texture>(L"RuinsTop", L"..\\Resources\\Texture\\TerrainCube\\New_ruins_floor.jpg");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetTexture(2, floorTexture);
			material->SetTexture(1, topTexture);
			meshRenderer->SetMaterial(material);
		}
		terraincube->AddComponent(meshRenderer);

		// 4. Scene에 추가
		activeScene->AddGameObject(terraincube);
	}
#pragma endregion


// 점령 중 UI

#pragma region UI
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->SetName(L"OccupationUI");
		obj->GetTransform()->SetLocalScale(Vec3(1500.f, 1000.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, 250.f, 155500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");

			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"OccupationUI", L"..\\Resources\\Texture\\OccupationUI.png");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		activeScene->AddGameObject(obj);
	}
#pragma endregion


// 먼지 파티클
#pragma region DustParticle
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> dustParticle = make_shared<GameObject>();
		wstring dustParticleName = L"DustParticle";
		dustParticle->SetName(dustParticleName);
		dustParticle->SetCheckFrustum(true);
		dustParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		dustParticle->AddComponent(make_shared<Transform>());
		dustParticle->GetTransform()->SetLocalPosition(Vec3(0, 1000.f, 0.f));
		dustParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<DustParticleSystem> dustParticleSystem = make_shared<DustParticleSystem>();
		dustParticle->AddComponent(dustParticleSystem);
		dustParticle->AddComponent(make_shared<TestParticleScript>());


		activeScene->AddGameObject(dustParticle);
	}
#pragma endregion

/*
// 빛기둥 파티클
#pragma region LightPillarParticle
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> lightPillarParticle = make_shared<GameObject>();
		wstring lightPillarParticleName = L"lightPillarParticle";
		lightPillarParticle->SetName(lightPillarParticleName);
		lightPillarParticle->SetCheckFrustum(true);
		lightPillarParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		lightPillarParticle->AddComponent(make_shared<Transform>());
		lightPillarParticle->GetTransform()->SetLocalPosition(Vec3(0.f, 2000.f, 0.f));
		lightPillarParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<LightPillarParticleSystem> lightPillarParticleSystem = make_shared<LightPillarParticleSystem>();
		lightPillarParticle->AddComponent(make_shared<TestParticleScript>());
		lightPillarParticle->AddComponent(lightPillarParticleSystem);

		activeScene->AddGameObject(lightPillarParticle);
	}
#pragma endregion
*/
// 물
#pragma region Water
	{
		shared_ptr<GameObject> water = make_shared<GameObject>();
		water->SetName(L"Water");
		water->AddComponent(make_shared<Transform>());
		water->AddComponent(make_shared<WaterScript>());
		water->GetTransform()->SetLocalScale(Vec3(10000.f, 1.f, 10000.f));
		water->GetTransform()->SetLocalPosition(Vec3(0.f, -300.f, 100000.f));
		water->SetStatic(true);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> waterMesh = GET_SINGLE(Resources)->LoadPlanMesh();
			meshRenderer->SetMesh(waterMesh);
		}
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Water");
			meshRenderer->SetMaterial(material);
		}

		water->AddComponent(meshRenderer);
		activeScene->AddGameObject(water);
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

		for (int i = 0; i < 10; ++i)
		{
			shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Gun_Bot\\Gun_Bot.fbx");
			vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

			// 겹치지 않는 랜덤 위치 생성
			Vec3 randomPos;
			bool validPosition = false;
			int maxAttempts = 100;

			while (!validPosition && maxAttempts > 0)
			{
				randomPos = Vec3(disX(gen), 0.0f, disZ(gen));
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

			gameObjects[0]->SetName(L"CyberCraps" + std::to_wstring(i + 1));
			gameObjects[0]->SetCheckFrustum(true);
			gameObjects[0]->AddComponent(make_shared<CrapScript>());
			gameObjects[0]->GetTransform()->SetLocalPosition(randomPos);
			gameObjects[0]->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
			activeScene->AddGameObject(gameObjects[0]);
		}
	}
#pragma endregion

// 점령 구역
#pragma region Occupation
	for (int i{}; i < 3; ++i)
	{
		shared_ptr<GameObject> occupation = make_shared<GameObject>();
		occupation->SetName(L"Occupation" + std::to_wstring(i + 1));
		occupation->AddComponent(make_shared<Transform>());
		occupation->AddComponent(make_shared<OccupationScript>());
		occupation->GetTransform()->SetLocalScale(Vec3(2000.f, 1.f, 2000.f));
		occupation->GetTransform()->SetLocalPosition(Vec3(0.f, i * 300, 0.f));
		occupation->SetStatic(false);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> occupationMesh = GET_SINGLE(Resources)->LoadPlanMesh();
			meshRenderer->SetMesh(occupationMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Occupation");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"OccupationTexture", L"..\\Resources\\Texture\\Occupation.png");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}

		occupation->AddComponent(meshRenderer);
		activeScene->AddGameObject(occupation);
	}
#pragma endregion

// 지형
#pragma region Stone
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (i == 0 || j == 0) {
				continue;
			}
			{
				shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\ruins\\Ruin_B.fbx");

				vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

				gameObjects[0]->SetName(L"Stone" + std::to_wstring(i + j + 2));
				gameObjects[0]->SetCheckFrustum(false);
				gameObjects[0]->SetStatic(false);
				gameObjects[0]->AddComponent(make_shared<Transform>());
				gameObjects[0]->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
				gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(0.0f + i * 4000, 40.f, 0.0f + j * 4000));
				gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(0.f, PI*(i+j), 0.f));
				activeScene->AddGameObject(gameObjects[0]);

			}
		}
	}
#pragma endregion

#pragma region Spot Light
	{
		// 1. Light 오브젝트 생성 
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Ancient_Ruin_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0.f, 4900.f, 0.f));

		// 2-1. Light 컴포넌트 추가 및 속성 설정
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);

		// 2-2. 스팟 라이트 방향 설정
		light->GetLight()->SetLightDirection(Vec3(0.f, -1.f, 0.f));

		float lightpower = 0.5f;
		// 3. 조명 색상 및 강도 조정 (따뜻한 황금빛)
		light->GetLight()->SetDiffuse(Vec3(1.0f, 0.85f, 0.6f) * lightpower);
		light->GetLight()->SetAmbient(Vec3(0.25f, 0.2f, 0.25f) * lightpower);
		light->GetLight()->SetSpecular(Vec3(0.9f, 0.8f, 0.6f) * lightpower);

		// 4. Scene에 추가
		activeScene->AddGameObject(light);

	}
#pragma endregion
}

RuinsScene::~RuinsScene() {}