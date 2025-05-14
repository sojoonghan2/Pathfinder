#include "pch.h"
#include "TestScene.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Application.h"
#include "Shader.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "ParticleSystem.h"
#include "CameraScript.h"
#include "TestHuman.h"
#include "Resources.h"
#include "MeshData.h"
#include "Animator.h"
#include "input.h"
#include "SphereCollider.h"

TestScene::TestScene() {}

TestScene::~TestScene() {}

void TestScene::Init()
{
	// ī�޶� ������ ������ ����.
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>());
		// ī�޶� ������ �� �ִ� ��ũ��Ʈ�� �߰�
		camera->AddComponent(make_shared<CameraScript>());
		camera->GetCamera()->SetFar(100000.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
		AddGameObject(camera);
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
		AddGameObject(light);
	}
#pragma endregion
	// �ӽ� �簢�� ��ü
	/*
	#pragma region UM
		// 1. �ӽ� ������Ʈ ���� �� ����
		shared_ptr<GameObject> um = make_shared<GameObject>();
		um->SetName(L"Um");
		um->SetCheckFrustum(true);

		// 2. Transform ������Ʈ �߰� �� ����
		um->AddComponent(make_shared<Transform>());
		um->GetTransform()->SetLocalScale(Vec3(WINDOWHEIGHT, WINDOWWIDTH, 1.f));
		um->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 1.f));

		// 3. MeshRenderer ����
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			// �簢�� �޽��� �����ͼ�
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			// �޽��������� �簢�� �޽��� ���ε�
			meshRenderer->SetMesh(mesh);
		}
		{
			// Texture��� �̸��� ���� ���̴��� ��������
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");
			// ���� �ؽ��� �̹����� ��������
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Um", L"..\\Resources\\Texture\\Um.png");
		   // ���͸����� ����
			shared_ptr<Material> material = make_shared<Material>();
			// ���͸��� ���̴��� �ؽ��ĸ� ���ε�
			material->SetShader(shader);
			material->SetTexture(0, texture);
			// �޽� �������� ���� ���� ���͸����� ���ε�
			meshRenderer->SetMaterial(material);
		}
		// ��ü�� �޽� ������ ���ε�
		um->AddComponent(meshRenderer);

		// 4. Scene�� �߰�
		AddGameObject(um);
	#pragma endregion

	*/
#pragma region TESTOBJ
	{
		TestHuman* Robot = new TestHuman();
		float robot_state = Robot->Get_state();

		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\char\\spaceops.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"TestObj");
			gameObject->SetCheckFrustum(false);
			gameObject->AddComponent(make_shared<Transform>());
			gameObject->AddComponent(make_shared<TestHuman>());
			gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
			gameObject->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
			AddGameObject(gameObject);
		}
	}
#pragma endregion
}