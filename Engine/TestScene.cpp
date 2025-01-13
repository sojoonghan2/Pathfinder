#include "pch.h"
#include "TestScene.h"
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
#include "Animator.h"
#include "input.h"
#include "SphereCollider.h"

TestScene::TestScene()
{
// ī�޶� ������ ������ ����.
#pragma region Camera
    {
        shared_ptr<GameObject> camera = make_shared<GameObject>();
        camera->SetName(L"Main_Camera");
        camera->AddComponent(make_shared<Transform>());
        camera->AddComponent(make_shared<Camera>());
        // ī�޶� ������ �� �ִ� ��ũ��Ʈ�� �߰�
        camera->AddComponent(make_shared<TestCameraScript>());
        camera->GetCamera()->SetFar(10000.f);
        camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
        uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
        camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
        activeScene->AddGameObject(camera);
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
    activeScene->AddGameObject(um);
#pragma endregion

*/
#pragma region TESTOBJ
    {
        shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\AnimationTest\\bi-rp_nathan_animated_003_walking.fbx");

        vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

        for (auto& gameObject : gameObjects)
        {
            gameObject->SetName(L"TestObj");
            gameObject->SetCheckFrustum(false);
            gameObject->GetTransform()->SetLocalPosition(Vec3(50.f, 0.f, 0.f));
            gameObject->GetTransform()->SetLocalScale(Vec3(20.f, 20.f, 20.f));
            gameObject->GetTransform()->SetLocalRotation(Vec3(0.f, 180.f, 0.f));
            activeScene->AddGameObject(gameObject);
        }
        
        if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
        {
            int32 count = GetAnimator()->GetAnimCount();
            int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

            int32 index = (currentIndex + 1) % count;
            GetAnimator()->Play(index);
        }

        if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
        {
            int32 count = GetAnimator()->GetAnimCount();
            int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

            int32 index = (currentIndex - 1 + count) % count;
            GetAnimator()->Play(index);
        }
    }
#pragma endregion
}

TestScene::~TestScene() {}