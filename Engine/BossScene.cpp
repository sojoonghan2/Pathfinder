#include "pch.h"
#include "BossScene.h"
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
#include "BossScript.h"

#include "SphereCollider.h"

BossScene::BossScene()
{
	// ��ǻƮ ���̴�, ��Ƽ������� �۾��� ����
#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV �� Texture ����
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// ������ �׷� (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

	// ī�޶�
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, FOV=45��
		camera->AddComponent(make_shared<CameraScript>());
		camera->AddComponent(make_shared<BossScript>());
		camera->GetCamera()->SetFar(100000.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
		activeScene->AddGameObject(camera);
	}
#pragma endregion

	// UI ī�޶�
#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // �� ����
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI�� ����
		activeScene->AddGameObject(camera);
	}
#pragma endregion

	// ��ī�� �ڽ�
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

	// �÷��̾�
#pragma region Player
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Player\\Player.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		gameObjects[0]->SetName(L"Player");
		gameObjects[0]->SetCheckFrustum(false);
		gameObjects[0]->AddComponent(make_shared<TestDragon>());
		gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(0.0f, -500.0f, -500.0f));
		gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(-1.5708f, 3.1416f, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
		gameObjects[0]->AddComponent(make_shared<PlayerScript>());

		activeScene->AddGameObject(gameObjects[0]);
	}
#pragma endregion

	// �ͷ��� ť��
#pragma region TerrainCube
	{
		// 1. �⺻ ������Ʈ ���� �� ����
		shared_ptr<GameObject> terraincube = make_shared<GameObject>();
		terraincube->AddComponent(make_shared<Transform>());
		terraincube->SetCheckFrustum(true);

		// 2. Transform ������Ʈ �߰� �� ����
		terraincube->AddComponent(make_shared<Transform>());
		// ���� �ӽ� ũ��
		terraincube->GetTransform()->SetLocalScale(Vec3(10000.f, 10000.f, 10000.f));
		// ���� �ӽ� ��ǥ
		terraincube->GetTransform()->SetLocalPosition(Vec3(0, 4900.f, 0.f));

		// 3. MeshRenderer ����
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TerrainCube");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"BossScene", L"..\\Resources\\Texture\\TerrainCube\\Boss.jpg");
			shared_ptr<Texture> floorTexture = GET_SINGLE(Resources)->Load<Texture>(L"BossSceneFloor", L"..\\Resources\\Texture\\TerrainCube\\BossFloor.jpg");
			shared_ptr<Texture> topTexture = GET_SINGLE(Resources)->Load<Texture>(L"BossSceneTop", L"..\\Resources\\Texture\\TerrainCube\\BossTop.jpg");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetTexture(1, floorTexture);
			material->SetTexture(2, topTexture);
			meshRenderer->SetMaterial(material);
		}
		terraincube->AddComponent(meshRenderer);

		// 4. Scene�� �߰�
		activeScene->AddGameObject(terraincube);
	}
#pragma endregion

	// ���� ����
#pragma region Directional Light
	{
		// 1. light ������Ʈ ����
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Directional_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

		// 2-1. light ������Ʈ �߰� �� �Ӽ� ����
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);

		// 2-2. DIRECTIONAL_LIGHT�� ��� ���� ���� ����
		light->GetLight()->SetLightDirection(Vec3(0.f, 0.f, 0.f));

		// 3. ���� ���� �� ���� ����
		light->GetLight()->SetDiffuse(Vec3(0.8f, 0.8f, 0.8f));
		light->GetLight()->SetAmbient(Vec3(0.8f, 0.8f, 0.8f));
		light->GetLight()->SetSpecular(Vec3(0.05f, 0.05f, 0.05f));

		// 4. Scene�� �߰�
		activeScene->AddGameObject(light);
	}
#pragma endregion

	// ����
#pragma region Boss
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Boss\\Boss.fbx");
		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		gameObjects[0]->SetName(L"Boss");
		gameObjects[0]->SetCheckFrustum(true);
		gameObjects[0]->AddComponent(make_shared<TestDragon>());
		gameObjects[0]->GetTransform()->SetLocalPosition(Vec3(0.0f, 500.0f, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalRotation(Vec3(1.3f, 0.0f, 0.0f));
		gameObjects[0]->GetTransform()->SetLocalScale(Vec3(1000.f, 1000.f, 1000.f));
		activeScene->AddGameObject(gameObjects[0]);
	}
#pragma endregion
}

BossScene::~BossScene() {}