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

// �ӽ� �簢�� ��ü
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
}

TestScene::~TestScene() {}