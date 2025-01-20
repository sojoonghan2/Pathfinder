#include "pch.h"
#include "ParticleScene.h"
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
#include "TestCameraScript.h"
#include "Resources.h"
#include "MeshData.h"

#include "ParticleSystem.h"
#include "BaseParticleSystem.h"
#include "IceParticleSystem.h"
#include "FireParticleSystem.h"
#include "CataclysmParticleSystem.h"
#include "RazerParticleSystem.h"
#include "OverDriveParticleSystem.h"
#include "TestPBRParticleSystem.h"

#include "TestDragon.h"
#include "TestPointLightScript.h"
#include "WaterScript.h"
#include "TestParticleScript.h"

#include "SphereCollider.h"

#define PARTICLEDEBUG	TRUE

ParticleScene::ParticleScene()
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
		camera->GetCamera()->SetFar(10000.f);
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

	if (PARTICLEDEBUG) LoadDebugParticle();
	else LoadMyParticle();
}

ParticleScene::~ParticleScene() {}

void ParticleScene::LoadMyParticle()
{
// ���� ��ƼŬ
#pragma region IceParticle
	// �׽�Ʈ ��� 100���� ����� fps 20���� ������
	// VRS �׽�Ʈ �� ��ƼŬ 100�� ���� �ɵ�
	{
		// ��ƼŬ ������Ʈ ����
		shared_ptr<GameObject> iceParticle = make_shared<GameObject>();
		wstring iceParticleName = L"IceParticle";
		iceParticle->SetName(iceParticleName);
		iceParticle->SetCheckFrustum(true);
		iceParticle->SetStatic(false);

		// ��ǥ ������Ʈ �߰�
		iceParticle->AddComponent(make_shared<Transform>());
		iceParticle->GetTransform()->SetLocalPosition(Vec3(-400.f, 100.f, -400.f));
		iceParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ��ƼŬ �ý��� ������Ʈ �߰�
		shared_ptr<IceParticleSystem> iceParticleSystem = make_shared<IceParticleSystem>();
		iceParticle->AddComponent(make_shared<TestParticleScript>());
		iceParticle->AddComponent(iceParticleSystem);

		activeScene->AddGameObject(iceParticle);
	}
#pragma endregion

// �� ��ƼŬ
#pragma region FireParticle
	{
		// ��ƼŬ ������Ʈ ����
		shared_ptr<GameObject> fireParticle = make_shared<GameObject>();
		wstring fireParticleName = L"FireParticle";
		fireParticle->SetName(fireParticleName);
		fireParticle->SetCheckFrustum(true);
		fireParticle->SetStatic(false);

		// ��ǥ ������Ʈ �߰�
		fireParticle->AddComponent(make_shared<Transform>());
		fireParticle->GetTransform()->SetLocalPosition(Vec3(400.f, 100.f, -400.f));
		fireParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ��ƼŬ �ý��� ������Ʈ �߰�
		shared_ptr<FireParticleSystem> fireParticleSystem = make_shared<FireParticleSystem>();
		fireParticle->AddComponent(make_shared<TestParticleScript>());
		fireParticle->AddComponent(fireParticleSystem);

		activeScene->AddGameObject(fireParticle);
	}
#pragma endregion

// ��ݺ� ��ƼŬ
#pragma region CataclysmParticle
	{
		// ��ƼŬ ������Ʈ ����
		shared_ptr<GameObject> cataclysmParticle = make_shared<GameObject>();
		wstring cataclysmParticleName = L"CataclysmParticle";
		cataclysmParticle->SetName(cataclysmParticleName);
		cataclysmParticle->SetCheckFrustum(true);
		cataclysmParticle->SetStatic(false);

		// ��ǥ ������Ʈ �߰�
		cataclysmParticle->AddComponent(make_shared<Transform>());
		cataclysmParticle->GetTransform()->SetLocalPosition(Vec3(0.f, -100.f, 400.f));
		cataclysmParticle->GetTransform()->SetLocalRotation(Vec3(0.0f, 90.0f, 0.0f));
		cataclysmParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ��ƼŬ �ý��� ������Ʈ �߰�
		shared_ptr<CataclysmParticleSystem> fireParticleSystem = make_shared<CataclysmParticleSystem>();
		cataclysmParticle->AddComponent(make_shared<TestParticleScript>());
		cataclysmParticle->AddComponent(fireParticleSystem);

		activeScene->AddGameObject(cataclysmParticle);
	}
#pragma endregion

// ������ ��ƼŬ
#pragma region RazerParticle
	{
		// ��ƼŬ ������Ʈ ����
		shared_ptr<GameObject> razerParticle = make_shared<GameObject>();
		wstring razerParticleName = L"RazerParticle";
		razerParticle->SetName(razerParticleName);
		razerParticle->SetCheckFrustum(true);
		razerParticle->SetStatic(false);

		// ��ǥ ������Ʈ �߰�
		razerParticle->AddComponent(make_shared<Transform>());
		razerParticle->GetTransform()->SetLocalPosition(Vec3(-400.f, 100.f, 400.f));
		razerParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ��ƼŬ �ý��� ������Ʈ �߰�
		shared_ptr<RazerParticleSystem> razerParticleSystem = make_shared<RazerParticleSystem>();
		razerParticle->AddComponent(make_shared<TestParticleScript>());
		razerParticle->AddComponent(razerParticleSystem);

		activeScene->AddGameObject(razerParticle);
	}
#pragma endregion

// ���� ����̺� ��ƼŬ
#pragma region OverDriveParticle
	{
		// ��ƼŬ ������Ʈ ����
		shared_ptr<GameObject> overDriveParticle = make_shared<GameObject>();
		wstring overDriveParticleName = L"OverDriveParticle";
		overDriveParticle->SetName(overDriveParticleName);
		overDriveParticle->SetCheckFrustum(true);
		overDriveParticle->SetStatic(false);

		// ��ǥ ������Ʈ �߰�
		overDriveParticle->AddComponent(make_shared<Transform>());
		overDriveParticle->GetTransform()->SetLocalPosition(Vec3(400.f, -100.f, 400.f));
		overDriveParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ��ƼŬ �ý��� ������Ʈ �߰�
		shared_ptr<OverDriveParticleSystem> overDriveParticleSystem = make_shared<OverDriveParticleSystem>();
		overDriveParticle->AddComponent(make_shared<TestParticleScript>());
		overDriveParticle->AddComponent(overDriveParticleSystem);

		activeScene->AddGameObject(overDriveParticle);
	}
#pragma endregion
}

void ParticleScene::LoadDebugParticle()
{
// �׽�Ʈ PBR ��ƼŬ
#pragma region TestPBRParticle
	{
		// ��ƼŬ ������Ʈ ����
		shared_ptr<GameObject> testPBRParticle = make_shared<GameObject>();
		wstring testPBRParticleName = L"testPBRParticle";
		testPBRParticle->SetName(testPBRParticleName);
		testPBRParticle->SetCheckFrustum(true);
		testPBRParticle->SetStatic(false);

		// ��ǥ ������Ʈ �߰�
		testPBRParticle->AddComponent(make_shared<Transform>());
		testPBRParticle->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 400.f));
		testPBRParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ��ƼŬ �ý��� ������Ʈ �߰�
		shared_ptr<TestPBRParticleSystem> testPBRParticleSystem = make_shared<TestPBRParticleSystem>();
		testPBRParticle->AddComponent(make_shared<TestParticleScript>());
		testPBRParticle->AddComponent(testPBRParticleSystem);

		activeScene->AddGameObject(testPBRParticle);
	}
#pragma endregion
}
