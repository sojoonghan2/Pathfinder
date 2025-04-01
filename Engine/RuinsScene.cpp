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
#include "CameraScript.h"
#include "Resources.h"
#include "MeshData.h"

#include "TestDragon.h"
#include "PlayerScript.h"
#include "WaterScript.h"
#include "OccupationScript.h"
#include "RuinsScript.h"
#include "CrapScript.h"
#include "RazerParticleScript.h"
#include "BulletScript.h"

#include "SphereCollider.h"

#include "DustParticleSystem.h"
#include "GlitterParticleSystem.h"
#include "TestParticleScript.h"
#include "LightPillarParticleSystem.h"
#include "TestGrenadeScript.h"
#include "FireParticleSystem.h"
#include "RazerParticleSystem.h"

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

// 카메라
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, FOV=45도
		camera->AddComponent(make_shared<CameraScript>());
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
		// 플레이어
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Player\\Player.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		auto playerScript = make_shared<PlayerScript>();


		for (auto gameObject : gameObjects)
		{
			gameObject->SetName(L"OBJ");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(Vec3(0.0f, -500.0f, 0.0f));
			gameObject->GetTransform()->SetLocalRotation(Vec3(-1.5708f, 3.1416f, 0.0f));
			gameObject->GetTransform()->SetLocalScale(Vec3(3.f, 3.f, 3.f));
			gameObject->AddComponent(playerScript);
			gameObject->AddComponent(make_shared<TestDragon>());
			activeScene->AddGameObject(gameObject);
		}

		// 총알
		for (int i{}; i < 50; ++i)
		{
			shared_ptr<GameObject> bullet = make_shared<GameObject>();
			bullet->SetName(L"Bullet" + std::to_wstring(i + 1));
			bullet->SetCheckFrustum(true);
			bullet->SetStatic(false);

			bullet->AddComponent(make_shared<Transform>());
			bullet->GetTransform()->SetLocalScale(Vec3(30.f, 30.f, 30.f));
			bullet->GetTransform()->SetParent(gameObjects[0]->GetTransform());
			bullet->GetTransform()->GetTransform()->RemoveParent();
			bullet->GetTransform()->SetLocalPosition(Vec3(0.f, 100000000000.f, 0.f));
			bullet->AddComponent(make_shared<BulletScript>(playerScript));

			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			{
				shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
				meshRenderer->SetMesh(sphereMesh);
			}
			{
				shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Bullet", L"..\\Resources\\Texture\\Bullet.png");

				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);
				meshRenderer->SetMaterial(material);
			}
			bullet->AddComponent(meshRenderer);

			activeScene->AddGameObject(bullet);
		}

		// 총알
		/*
		for (int i{}; i < 50; ++i)
		{
			shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Bullet\\Bullet.fbx");
			vector<shared_ptr<GameObject>> bullets = meshData->Instantiate();

			for (auto bullet : bullets)
			{
				bullet->SetName(L"Bullet" + std::to_wstring(i + 1));
				bullet->SetCheckFrustum(true);
				bullet->SetStatic(false);

				bullet->AddComponent(make_shared<Transform>());
				bullet->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
				bullet->GetTransform()->SetParent(gameObjects[0]->GetTransform());
				bullet->GetTransform()->GetTransform()->RemoveParent();
				bullet->GetTransform()->SetLocalPosition(Vec3(0.f, 100000000000.f, 0.f));
				bullet->AddComponent(make_shared<BulletScript>(playerScript));
				activeScene->AddGameObject(bullet);
			}
		}
		*/

		// 수류탄
		shared_ptr<GameObject> grenade = make_shared<GameObject>();
		grenade->SetName(L"Grenade");
		grenade->SetCheckFrustum(true);
		grenade->SetStatic(false);

		grenade->AddComponent(make_shared<Transform>());
		grenade->GetTransform()->SetLocalScale(Vec3(30.f, 30.f, 30.f));
		grenade->GetTransform()->SetParent(gameObjects[0]->GetTransform());
		grenade->GetTransform()->GetTransform()->RemoveParent();
		grenade->GetTransform()->SetLocalPosition(Vec3(0.f, 100000000000.f, 0.f));
		grenade->AddComponent(make_shared<TestGrenadeScript>(playerScript));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Grenade", L"..\\Resources\\Texture\\Grenade.jpg");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		grenade->AddComponent(meshRenderer);

		shared_ptr<FireParticleSystem> grenadeParticleSystem = make_shared<FireParticleSystem>();
		grenadeParticleSystem->SetParticleScale(100.f, 80.f);
		grenade->AddComponent(grenadeParticleSystem);

		activeScene->AddGameObject(grenade);

		// 레이저 파티클
		shared_ptr<GameObject> razerParticle = make_shared<GameObject>();
		wstring razerParticleName = L"RazerParticle";
		razerParticle->SetName(razerParticleName);
		razerParticle->SetCheckFrustum(true);
		razerParticle->SetStatic(false);

		razerParticle->AddComponent(make_shared<Transform>());
		razerParticle->GetTransform()->SetParent(gameObjects[0]->GetTransform());
		razerParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
		razerParticle->GetTransform()->SetLocalPosition(Vec3(0.0f, 100.0f, 110.0f));
		razerParticle->AddComponent(make_shared<RazerParticleScript>(playerScript));

		shared_ptr<RazerParticleSystem> razerParticleSystem = make_shared<RazerParticleSystem>();
		Vec3 look = gameObjects[0]->GetTransform()->GetLook();
		look.Normalize();

		Vec4 dir{ look.x, look.y, look.z, 0.0f };
		razerParticleSystem->SetEmitDirection(dir);
		razerParticle->AddComponent(razerParticleSystem);

		activeScene->AddGameObject(razerParticle);

		// 조준점
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->SetName(L"CrosshairUI");
		obj->GetTransform()->SetLocalScale(Vec3(500.f, 500.f, 500.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 1.f));
		shared_ptr<MeshRenderer> CrosshairmeshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			CrosshairmeshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");

			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Crosshair", L"..\\Resources\\Texture\\Crosshair.png");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			CrosshairmeshRenderer->SetMaterial(material);
		}
		obj->AddComponent(CrosshairmeshRenderer);
		obj->SetRenderOff();
		activeScene->AddGameObject(obj);
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
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Ruins", L"..\\Resources\\Texture\\TerrainCube\\New_ruins.jpg");
			shared_ptr<Texture> floorTexture = GET_SINGLE(Resources)->Load<Texture>(L"RuinsFloor", L"..\\Resources\\Texture\\TerrainCube\\New_ruins_floor.jpg");
			shared_ptr<Texture> topTexture = GET_SINGLE(Resources)->Load<Texture>(L"RuinsTop", L"..\\Resources\\Texture\\TerrainCube\\New_ruins.jpg");

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

// 점령 중 UI
#pragma region UI
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->SetName(L"OccupationUI");
		obj->GetTransform()->SetLocalScale(Vec3(1500.f, 1000.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, 250.f, 500.f));
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

// 천장에서 빛이 새어나오는 유적지 조명
#pragma region Spot Light
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetName(L"OBJ");
		obj->SetCheckFrustum(true);
		obj->SetStatic(false);

		obj->AddComponent(make_shared<Transform>());
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
		obj->SetRenderOff();
		activeScene->AddGameObject(obj);

		// 1. Light 오브젝트 생성 
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Ancient_Ruin_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0.f, 10000.f, 0.f));

		// 2-1. Light 컴포넌트 추가 및 속성 설정
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::SPOT_LIGHT);

		// 2-2. 스팟 라이트 방향 설정
		light->GetLight()->SetLightDirection(Vec3(0.f, -1.f, 0.f));

		// 2-3. 스팟 라이트 원뿔 각도 설정
		light->GetLight()->SetLightAngle(10.f);

		// 2-4. 빛의 도달 범위 증가
		light->GetLight()->SetLightRange(11000.f);

		// 3. 조명 색상 및 강도 조정 (따뜻한 황금빛)
		light->GetLight()->SetDiffuse(Vec3(1.0f, 0.85f, 0.6f));
		light->GetLight()->SetAmbient(Vec3(0.25f, 0.2f, 0.25f));
		light->GetLight()->SetSpecular(Vec3(0.9f, 0.8f, 0.6f));

		// 4. Scene에 추가
		activeScene->AddGameObject(light);

		// 5. 환경광 추가 (더 따뜻한 분위기 연출)
		shared_ptr<GameObject> ambientLight = make_shared<GameObject>();
		ambientLight->SetName(L"Ancient_Ambient_Light");
		ambientLight->AddComponent(make_shared<Transform>());
		ambientLight->AddComponent(make_shared<Light>());
		ambientLight->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		ambientLight->GetLight()->SetDiffuse(Vec3(0.4f, 0.4f, 0.4f));
		ambientLight->GetLight()->SetAmbient(Vec3(0.4f, 0.2f, 0.25f));
		ambientLight->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
		activeScene->AddGameObject(ambientLight);
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

// 물
#pragma region Water
	{
		shared_ptr<GameObject> water = make_shared<GameObject>();
		water->SetName(L"Water");
		water->AddComponent(make_shared<Transform>());
		water->AddComponent(make_shared<WaterScript>());
		water->GetTransform()->SetLocalScale(Vec3(10000.f, 1.f, 10000.f));
		water->GetTransform()->SetLocalPosition(Vec3(0.f, 300.f, 50.f));
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
				gameObjects[0]->AddComponent(make_shared<Transform>());
				gameObjects[0]->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
				gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(0.0f + i * 4000, 40.f, 0.0f + j * 4000));
				gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(0.f, PI*(i+j), 0.f));
				activeScene->AddGameObject(gameObjects[0]);

			}
		}
	}
#pragma endregion

}

RuinsScene::~RuinsScene() {}