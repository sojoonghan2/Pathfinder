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
#include "TestPointLightScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "SphereCollider.h"
#include "MeshData.h"
#include "TestDragon.h"

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
	if (_activeScene)
		_activeScene->Render();
}

void SceneManager::LoadScene(wstring sceneName)
{
	// TODO : ���� Scene ����
	// TODO : ���Ͽ��� Scene ���� �ε�

	_activeScene = LoadMainScene();

	_activeScene->Awake();
	_activeScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// ���� ������ ����
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

	// ViewSpace���� Picking ����
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

		// ViewSpace������ Ray ����
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace������ Ray ����
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace���� ����
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
			return obj; // �̸��� ��ġ�ϴ� ������Ʈ�� ã���� ��ȯ
		}
	}

	return nullptr; // ã�� ���ϸ� nullptr ��ȯ
}

shared_ptr<Scene> SceneManager::LoadMainScene()
{
// ���̾�
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

// ��ǻƮ ���̴�, ��Ƽ������� �۾��� �����ϴٴµ� ���� �� ��
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

	// Scene ����
	shared_ptr<Scene> scene = make_shared<Scene>();
	
// ī�޶�
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, FOV=45��
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->GetCamera()->SetFar(10000.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
		scene->AddGameObject(camera);
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
		scene->AddGameObject(camera);
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
		scene->AddGameObject(skybox);
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
		terraincube->GetTransform()->SetLocalScale(Vec3(2000.f, 2000.f, 2000.f));
		terraincube->GetTransform()->SetLocalPosition(Vec3(0, 0.f, 0.f));

		// 4. MeshRenderer ����
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

		// 5. Scene�� �߰�
		scene->AddGameObject(terraincube);
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
		scene->AddGameObject(obj);

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
		scene->AddGameObject(light);
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
		scene->AddGameObject(obj);
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
		light->GetLight()->SetAmbient(Vec3(0.2f, 0.2f, 0.2f));
		light->GetLight()->SetSpecular(Vec3(0.05f, 0.05f, 0.05f));

		// 4. Scene�� �߰�
		scene->AddGameObject(light);
	}
#pragma endregion

// FBX
#pragma region FBX
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\TV\\SmartTV.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		// Save�� �� �ȵ�, MeshData�� Save�� Load�� �����ؾ� ��, �ѹ� �ε� �� �ٽ� �ε��Ϸ��� msh ������ ��
		//meshData->SaveOrLoad(L"..\\Resources\\MeshData\\Dragon.msh", L"..\\Resources\\FBX\\Dragon.fbx");

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"Test");
			gameObject->SetCheckFrustum(true);
			gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 300.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			scene->AddGameObject(gameObject);
			gameObject->AddComponent(make_shared<TestDragon>());
		}
	}
#pragma endregion


	return scene;
}