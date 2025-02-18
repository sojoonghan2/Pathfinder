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
#include "TestPointLightScript.h"
#include "WaterScript.h"
#include "LuckyScript.h"
#include "ModuleScript.h"

#include "SphereCollider.h"
#include "GameModule.h"

LuckyScene::LuckyScene()
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
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->AddComponent(make_shared<LuckyScript>());
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
		terraincube->GetTransform()->SetLocalScale(Vec3(2000.f, 2000.f, 2000.f));
		// ���� �ӽ� ��ǥ
		terraincube->GetTransform()->SetLocalPosition(Vec3(0, 800.f, 0.f));

		// 3. MeshRenderer ����
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"TerrainCube");
			meshRenderer->SetMaterial(material->Clone());
		}
		terraincube->AddComponent(meshRenderer);

		// 4. Scene�� �߰�
		activeScene->AddGameObject(terraincube);
	}
#pragma endregion

// �� ������Ʈ�� ����Ʈ ����
#pragma region Object & Point Light
	{
		// 1. �⺻ ������Ʈ ���� �� ����
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetName(L"OBJ");
		// �������� �ø� ����
		obj->SetCheckFrustum(true);
		// ����, ���� ����
		obj->SetStatic(false);

		// 2. Transform ������Ʈ �߰� �� ����
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0, 0.f, 0.f));
		obj->AddComponent(make_shared<TestPointLightScript>());

		// 3. Collider ����
		obj->AddComponent(make_shared<SphereCollider>());
		dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetRadius(0.5f);
		dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetCenter(Vec3(0.f, 0.f, 0.f));

		// 4. MeshRenderer ����
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			// GameObject �̸��� ���� ���͸����� ã�� Ŭ�� �� �޽� �������� Set
			// Resources.cpp�� ���� ���̴��� ���͸����� ������ Ȯ���غ� �� ����
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
			meshRenderer->SetMaterial(material->Clone());
		}
		obj->AddComponent(meshRenderer);

		// 5. Scene�� �߰�
		activeScene->AddGameObject(obj);

		// 1. light ������Ʈ ���� 
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Point_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0, 0.f, 0.f));

		// 2-1. light ������Ʈ �߰� �� �Ӽ� ����
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::POINT_LIGHT);
		light->AddComponent(make_shared<TestPointLightScript>());

		// 2-2. ������ Ư�� ����
		light->GetLight()->SetLightRange(1000.f);

		// 3. ���� ���� �� ���� ����
		light->GetLight()->SetDiffuse(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetAmbient(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetSpecular(Vec3(0.5f, 0.5f, 0.5f));

		// 4. Scene�� �߰�
		activeScene->AddGameObject(light);
	}
#pragma endregion

// *********************************************
// UI �׽�Ʈ
// 0. ���� ���� ��ġ ����
// 1. �븻 ���� ����
// 2. ���� ����
// 3. �л걤 ���
// 4. �ݻ籤 ���
// 5. �׸���
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

// �κ�
#pragma region BOT
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Bot\\Bot.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		// Save�� �� �ȵ�, MeshData�� Save�� Load�� �����ؾ� ��, �ѹ� �ε� �� �ٽ� �ε��Ϸ��� msh ������ ��
		//meshData->SaveOrLoad(L"..\\Resources\\MeshData\\Dragon.msh", L"..\\Resources\\FBX\\Dragon.fbx");

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"Bot");
			gameObject->SetCheckFrustum(true);
			gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, -200.f, 300.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
			gameObject->GetTransform()->SetLocalRotation(Vec3(-1.7f, 0.f, 0.f));
			activeScene->AddGameObject(gameObject);
		}
	}
#pragma endregion

// �÷��̾�
#pragma region PLAYER
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Halo 5 Noble\\Halo 5 Noble.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		// Save�� �� �ȵ�, MeshData�� Save�� Load�� �����ؾ� ��, �ѹ� �ε� �� �ٽ� �ε��Ϸ��� msh ������ ��
		//meshData->SaveOrLoad(L"..\\Resources\\MeshData\\Dragon.msh", L"..\\Resources\\FBX\\Dragon.fbx");

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"Player");
			gameObject->SetCheckFrustum(true);
			gameObject->GetTransform()->SetLocalPosition(Vec3(200.f, -200.f, 300.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
			gameObject->GetTransform()->SetLocalRotation(Vec3(-1.7f, 0.f, 0.f));
			activeScene->AddGameObject(gameObject);
		}
	}
#pragma endregion

// ��
#pragma region Water
	{
		shared_ptr<GameObject> water = make_shared<GameObject>();
		water->SetName(L"Water");
		water->AddComponent(make_shared<Transform>());
		water->AddComponent(make_shared<WaterScript>());
		water->GetTransform()->SetLocalScale(Vec3(2000.f, 1.f, 2000.f));
		water->GetTransform()->SetLocalPosition(Vec3(0.f, 10.f, 50.f));
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

// ���
#pragma region Module
	for (int i{}; i < 3; ++i)
	{
		shared_ptr<GameModule> obj = make_shared<GameModule>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->AddComponent(make_shared<ModuleScript>());
		obj->GetTransform()->SetLocalScale(Vec3(300.f, 500.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-400.f + (i * 400), 0.f, 1.f));
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