#include "pch.h"
#include "FactoryScene.h"
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
#include "FactoryScript.h"
#include "TestGrenadeScript.h"
#include "RazerParticleScript.h"
#include "GunScript.h"
#include "BulletScript.h"
#include "TestParticleScript.h"

#include "SphereCollider.h"
#include "GeneratorScript.h"

#include "IceParticleSystem.h"
#include "RazerParticleSystem.h"
#include "GunFlameParticleSystem.h"
#include "FireParticleSystem.h"
#include "PortalFrameParticleSystem.h"
#include "TestPBRParticleSystem.h"

FactoryScene::FactoryScene() {}

FactoryScene::~FactoryScene() {}

void FactoryScene::Init()
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
		camera->AddComponent(make_shared<FactoryScript>());
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
			gameObject->SetName(L"Player");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(Vec3(0.0f, -500.0f, 0.0f));
			gameObject->GetTransform()->SetLocalRotation(Vec3(-PI / 2, PI, 0.0f));
			gameObject->GetTransform()->SetLocalScale(Vec3(3.f, 3.f, 3.f));
			gameObject->AddComponent(playerScript);
			gameObject->AddComponent(make_shared<TestDragon>());

			gameObject->AddComponent(make_shared<SphereCollider>());
			dynamic_pointer_cast<SphereCollider>(gameObject->GetCollider())->SetRadius(100.f);
			dynamic_pointer_cast<SphereCollider>(gameObject->GetCollider())->SetCenter(Vec3(0.f, 100.f, 0.f));

			activeScene->AddGameObject(gameObject);

			shared_ptr<GameObject> wire = make_shared<GameObject>();
			wstring name = L"wire";
			wire->SetName(name);

			wire->AddComponent(make_shared<Transform>());
			wire->GetTransform()->SetParent(gameObject->GetTransform());
			wire->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 100.f));
			wire->GetTransform()->SetLocalScale(Vec3(125.f, 125.f, 125.f));

			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			{
				shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
				meshRenderer->SetMesh(sphereMesh);
			}
			{
				shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Debug");
				meshRenderer->SetMaterial(material->Clone());
			}
			wire->AddComponent(meshRenderer);

			//activeScene->AddGameObject(wire);

			shared_ptr<GameObject> particleObject = make_shared<GameObject>();
			particleObject->SetName(L"GunFlameParticle");
			particleObject->AddComponent(make_shared<Transform>());
			particleObject->GetTransform()->SetParent(gameObject->GetTransform());
			particleObject->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 150.f));
			shared_ptr<GunFlameParticleSystem> gunFlameParticleSystem = make_shared<GunFlameParticleSystem>();
			gunFlameParticleSystem->SetParticleScale(50.f, 50.f);
			particleObject->AddComponent(gunFlameParticleSystem);

			activeScene->AddGameObject(particleObject);
		}

		// 총
		shared_ptr<MeshData> gunmeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Gun\\Gun.fbx");
		vector<shared_ptr<GameObject>> gungameObjects = gunmeshData->Instantiate();

		for (auto gameObject : gungameObjects)
		{
			gameObject->SetName(L"Gun");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetParent(gameObjects[0]->GetTransform());
			gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
			gameObject->GetTransform()->SetLocalRotation(Vec3(PI / 2, -0.4f, 1.f));
			gameObject->GetTransform()->SetLocalPosition(Vec3(42.f, 58.f, -3.f));
			gameObject->AddComponent(make_shared<GunScript>());

			activeScene->AddGameObject(gameObject);
		}

		// 총알
		for (int i{}; i < 50; ++i)
		{
			shared_ptr<GameObject> bullet = make_shared<GameObject>();
			bullet->SetName(L"Bullet" + std::to_wstring(i));
			bullet->SetCheckFrustum(true);
			bullet->SetStatic(false);

			bullet->AddComponent(make_shared<Transform>());
			bullet->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
			bullet->GetTransform()->SetParent(gameObjects[0]->GetTransform());
			bullet->GetTransform()->GetTransform()->RemoveParent();
			bullet->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
			bullet->AddComponent(make_shared<BulletScript>(playerScript));

			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			{
				shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
				meshRenderer->SetMesh(sphereMesh);
			}
			{
				shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
				shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Bullet", L"..\\Resources\\Texture\\Skill\\Bullet.png");

				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(shader);
				material->SetTexture(0, texture);
				meshRenderer->SetMaterial(material);
			}

			bullet->AddComponent(make_shared<SphereCollider>());
			dynamic_pointer_cast<SphereCollider>(bullet->GetCollider())->SetRadius(10.f);
			dynamic_pointer_cast<SphereCollider>(bullet->GetCollider())->SetCenter(Vec3(0.f, 0.f, 0.f));
			dynamic_pointer_cast<SphereCollider>(bullet->GetCollider())->SetEnable(false);

			bullet->AddComponent(meshRenderer);
			bullet->SetRender(false);

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
		grenade->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		grenade->AddComponent(make_shared<TestGrenadeScript>(playerScript));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Grenade", L"..\\Resources\\Texture\\Skill\\Grenade.jpg");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		grenade->AddComponent(meshRenderer);

		shared_ptr<FireParticleSystem> grenadeParticleSystem = make_shared<FireParticleSystem>();
		grenadeParticleSystem->SetParticleScale(100.f, 80.f);
		grenade->AddComponent(grenadeParticleSystem);

		grenade->AddComponent(make_shared<SphereCollider>());
		dynamic_pointer_cast<SphereCollider>(grenade->GetCollider())->SetRadius(500.f);
		dynamic_pointer_cast<SphereCollider>(grenade->GetCollider())->SetCenter(Vec3(0.f, 0.f, 0.f));
		dynamic_pointer_cast<SphereCollider>(grenade->GetCollider())->SetEnable(false);

		grenade->SetRender(false);
		activeScene->AddGameObject(grenade);

		// 레이저 파티클
		shared_ptr<GameObject> razerParticle = make_shared<GameObject>();
		razerParticle->SetName(L"Razer");
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
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
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
		obj->SetRender(false);
		activeScene->AddGameObject(obj);

		shared_ptr<GameObject> playerFace = make_shared<GameObject>();
		playerFace->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		playerFace->AddComponent(make_shared<Transform>());
		playerFace->SetName(L"playerFace");
		playerFace->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		playerFace->GetTransform()->SetLocalPosition(Vec3(-530.f, -300.f, 1.f));
		shared_ptr<MeshRenderer> playerFacemeshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			playerFacemeshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture{};
			texture = GET_SINGLE(Resources)->Load<Texture>(L"playerFace", L"..\\Resources\\Texture\\playerFace.PNG");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			playerFacemeshRenderer->SetMaterial(material);
		}
		playerFace->AddComponent(playerFacemeshRenderer);
		activeScene->AddGameObject(playerFace);

		// hp
		shared_ptr<GameObject> hpBase = make_shared<GameObject>();
		hpBase->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		hpBase->AddComponent(make_shared<Transform>());
		hpBase->SetName(L"HPBase");
		hpBase->GetTransform()->SetLocalScale(Vec3(200.f, 100.f, 100.f));
		hpBase->GetTransform()->SetLocalPosition(Vec3(-350.f, -300.f, 1.f));
		shared_ptr<MeshRenderer> hpBasemeshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			hpBasemeshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture{};
			texture = GET_SINGLE(Resources)->Load<Texture>(L"HPBase", L"..\\Resources\\Texture\\HPBase.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			hpBasemeshRenderer->SetMaterial(material);
		}
		hpBase->AddComponent(hpBasemeshRenderer);
		activeScene->AddGameObject(hpBase);

		shared_ptr<GameObject> hp = make_shared<GameObject>();
		hp->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		hp->AddComponent(make_shared<Transform>());
		hp->SetName(L"HP");
		hp->GetTransform()->SetLocalScale(Vec3(180.f, 85.f, 100.f));
		hp->GetTransform()->SetLocalPosition(Vec3(-350.f, -300.f, 1.f));
		shared_ptr<MeshRenderer> hpmeshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			hpmeshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture{};
			texture = GET_SINGLE(Resources)->Load<Texture>(L"HP", L"..\\Resources\\Texture\\HP.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			hpmeshRenderer->SetMaterial(material);
		}
		hp->AddComponent(hpmeshRenderer);
		activeScene->AddGameObject(hp);
	}
#pragma endregion

	// 스킬 아이콘
#pragma region Icon
	for (int i{}; i < 3; ++i)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		if (i == 0) obj->SetName(L"DashUI");
		else if (i == 1) obj->SetName(L"GrenadeUI");
		else if (i == 2) obj->SetName(L"RazerUI");
		obj->GetTransform()->SetLocalScale(Vec3(130.f, 130.f, 130.f));
		obj->GetTransform()->SetLocalPosition(Vec3(270.f + i * 140, -300.f, 1.f));
		shared_ptr<MeshRenderer> CrosshairmeshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			CrosshairmeshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture{};
			if (i == 0) texture = GET_SINGLE(Resources)->Load<Texture>(L"DashUI", L"..\\Resources\\Texture\\Skill\\Dash.png");
			else if (i == 1) texture = GET_SINGLE(Resources)->Load<Texture>(L"GrenadeUI", L"..\\Resources\\Texture\\Skill\\Grenade.png");
			else if (i == 2) texture = GET_SINGLE(Resources)->Load<Texture>(L"RazerUI", L"..\\Resources\\Texture\\Skill\\Razer.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			CrosshairmeshRenderer->SetMaterial(material);
		}
		obj->AddComponent(CrosshairmeshRenderer);
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
		terraincube->GetTransform()->SetLocalScale(Vec3(11000.f, 11000.f, 11000.f));
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
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Factory", L"..\\Resources\\Texture\\TerrainCube\\Factory.jpg");
			shared_ptr<Texture> floorTexture = GET_SINGLE(Resources)->Load<Texture>(L"FactoryFloor", L"..\\Resources\\Texture\\TerrainCube\\FactoryFloor.jpg");
			shared_ptr<Texture> topTexture = GET_SINGLE(Resources)->Load<Texture>(L"FactoryTop", L"..\\Resources\\Texture\\TerrainCube\\FactoryTop.jpg");

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

	// 로봇
#pragma region FactoryMonster
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Monster\\Monster.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		gameObjects[0]->SetName(L"FactoryMonster");
		gameObjects[0]->SetCheckFrustum(true);
		gameObjects[0]->GetMeshRenderer()->GetMesh()->SetVrs(true);
		gameObjects[0]->GetMeshRenderer()->GetMesh()->SetRatingTier(D3D12_VARIABLE_SHADING_RATE_TIER_2);
		gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(0.0f, -100.0f, 2000.0f));
		gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(-PI / 2, 0.0f, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		gameObjects[0]->AddComponent(make_shared<SphereCollider>());
		dynamic_pointer_cast<SphereCollider>(gameObjects[0]->GetCollider())->SetRadius(200.f);
		dynamic_pointer_cast<SphereCollider>(gameObjects[0]->GetCollider())->SetCenter(Vec3(0.f, 100.f, 0.f));

		activeScene->AddGameObject(gameObjects[0]);

		// hp
		shared_ptr<GameObject> hpBase = make_shared<GameObject>();
		hpBase->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		hpBase->AddComponent(make_shared<Transform>());
		hpBase->SetName(L"FactoryMonsterHPBase");
		hpBase->GetTransform()->SetLocalScale(Vec3(500.f, 50.f, 100.f));
		hpBase->GetTransform()->SetLocalPosition(Vec3(0.f, 300.f, 1.f));
		shared_ptr<MeshRenderer> hpBasemeshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			hpBasemeshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture{};
			texture = GET_SINGLE(Resources)->Load<Texture>(L"FactoryMonsterHPBase", L"..\\Resources\\Texture\\FactoryMonsterHPBase.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			hpBasemeshRenderer->SetMaterial(material);
		}
		hpBase->AddComponent(hpBasemeshRenderer);
		activeScene->AddGameObject(hpBase);

		shared_ptr<GameObject> hp = make_shared<GameObject>();
		hp->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		hp->AddComponent(make_shared<Transform>());
		hp->SetName(L"FactoryMonsterHP");
		hp->GetTransform()->SetLocalScale(Vec3(480.f, 35.f, 100.f));
		hp->GetTransform()->SetLocalPosition(Vec3(0.f, 300.f, 1.f));
		shared_ptr<MeshRenderer> hpmeshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			hpmeshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture{};
			texture = GET_SINGLE(Resources)->Load<Texture>(L"FactoryMonsterHP", L"..\\Resources\\Texture\\FactoryMonsterHP.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			hpmeshRenderer->SetMaterial(material);
		}
		hp->AddComponent(hpmeshRenderer);
		activeScene->AddGameObject(hp);
	}
#pragma endregion
	

	// 전역 조명
#pragma region Directional Light
	{
		// 1. Light 오브젝트 생성 
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Ancient_Ruin_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0.f, 9900.f, 0.f));

		// 2-1. Light 컴포넌트 추가 및 속성 설정
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);

		// 2-2. 스팟 라이트 방향 설정
		light->GetLight()->SetLightDirection(Vec3(0.f, -1.f, 0.f));

		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.4f, 0.4f, 0.4f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));

		// 4. Scene에 추가
		activeScene->AddGameObject(light);

	}
#pragma endregion

	// 팩토리 맵
#pragma region FactoryMap
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\FactoryMap\\FactoryMap.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		for (auto gameObject : gameObjects)
		{
			gameObject->SetName(L"FactoryMap");
			gameObject->SetCheckFrustum(true);
			gameObject->SetStatic(true);
			gameObject->AddComponent(make_shared<Transform>());
			gameObject->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
			gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, -100.f, 0.f));
			gameObject->GetTransform()->SetLocalRotation(Vec3(-PI / 2, PI, 0.0f));
			activeScene->AddGameObject(gameObject);
		}
	}
#pragma endregion

	// 공급기
#pragma region Generator
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Generator\\Generator.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		gameObjects[0]->SetName(L"Generator");
		gameObjects[0]->SetCheckFrustum(true);
		gameObjects[0]->AddComponent(make_shared<GeneratorScript>());
		gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(2000.0f, -200.0f, 2000.0f));
		gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(-1.6f, 0.0f, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

		shared_ptr<GameObject> particleObj = make_shared<GameObject>();
		particleObj->SetName(L"IceParticleSystem");
		particleObj->AddComponent(make_shared<Transform>());
		particleObj->GetTransform()->SetParent(gameObjects[0]->GetTransform());
		particleObj->GetTransform()->SetLocalPosition(Vec3(0.0f, 0.0f, 500.0f));
		particleObj->AddComponent(make_shared<IceParticleSystem>());
		particleObj->AddComponent(make_shared<GeneratorScript>());
		activeScene->AddGameObject(particleObj);

		activeScene->AddGameObject(gameObjects[0]);
	}
#pragma endregion

#pragma region PotalParticle
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> testPBRParticle = make_shared<GameObject>();
		wstring testPBRParticleName = L"potalParticle";
		testPBRParticle->SetName(testPBRParticleName);
		testPBRParticle->SetCheckFrustum(true);
		testPBRParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		testPBRParticle->AddComponent(make_shared<Transform>());
		testPBRParticle->GetTransform()->SetLocalPosition(Vec3(0.f, 500.f, 4000.f));
		testPBRParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<TestPBRParticleSystem> testPBRParticleSystem = make_shared<TestPBRParticleSystem>();
		testPBRParticle->AddComponent(make_shared<TestParticleScript>());
		testPBRParticle->AddComponent(testPBRParticleSystem);

		activeScene->AddGameObject(testPBRParticle);
	}
#pragma endregion

#pragma region PortalFrameParticle
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> portalFrameParticle = make_shared<GameObject>();
		wstring portalFrameParticleName = L"portalFrameParticle";
		portalFrameParticle->SetName(portalFrameParticleName);
		portalFrameParticle->SetCheckFrustum(true);
		portalFrameParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		portalFrameParticle->AddComponent(make_shared<Transform>());
		portalFrameParticle->GetTransform()->SetLocalPosition(Vec3(0.f, 500.f, 4000.f));
		portalFrameParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<PortalFrameParticleSystem> portalFrameParticleSystem = make_shared<PortalFrameParticleSystem>();
		portalFrameParticle->AddComponent(make_shared<TestParticleScript>());
		portalFrameParticle->AddComponent(portalFrameParticleSystem);

		activeScene->AddGameObject(portalFrameParticle);
	}
#pragma endregion
}