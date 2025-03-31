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
#include "CameraScript.h"
#include "Resources.h"
#include "MeshData.h"

#include "ParticleSystem.h"
#include "BaseParticleSystem.h"
#include "IceParticleSystem.h"
#include "FireParticleSystem.h"
#include "CataclysmParticleSystem.h"
#include "RazerParticleSystem.h"
#include "OverDriveParticleSystem.h"
#include "GlitterParticleSystem.h"
#include "TestPBRParticleSystem.h"

#include "TestDragon.h"
#include "PlayerScript.h"
#include "WaterScript.h"
#include "TestParticleScript.h"
#include "ModuleScript.h"
#include "MasterScript.h"
#include "TestGrenadeScript.h"
#include "RuinsScript.h"
#include "TestOtherPlayerScript.h"
#include "RazerParticleScript.h"

#include "SphereCollider.h"
#include "RectangleCollider.h"

#include "GameModule.h"
#include "Player.h"

#define PARTICLEDEBUG	FALSE

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
        camera->AddComponent(make_shared<CameraScript>());
        camera->GetCamera()->SetFar(100000.f);
        camera->GetTransform()->SetLocalPosition(Vec3(0.f, 500.f, 0.f));
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
        terraincube->GetTransform()->SetLocalScale(Vec3(10000.f, 10000.f, 10000.f));
        // ���� �ӽ� ��ǥ
        terraincube->GetTransform()->SetLocalPosition(Vec3(0.f, 4900.f, 0.f));

        // 3. MeshRenderer ����
        shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
        {
            shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
            meshRenderer->SetMesh(sphereMesh);
        }
        {
            shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TerrainCube");
            shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"TestParticle", L"..\\Resources\\Texture\\TerrainCube\\TestParticle.jpg");
            shared_ptr<Texture> floorTexture = GET_SINGLE(Resources)->Load<Texture>(L"TestParticleFloor", L"..\\Resources\\Texture\\TerrainCube\\TestParticleFloor.jpg");
            shared_ptr<Texture> topTexture = GET_SINGLE(Resources)->Load<Texture>(L"TestParticleTop", L"..\\Resources\\Texture\\TerrainCube\\TestParticleTop.jpg");

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

// �÷��̾�
#pragma region Player
    {
        // �÷��̾�
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

        // ����ź
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

        // ������ ��ƼŬ
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
        
        // ��ƼŬ�� ������ ù �� ���鶧�� �ƴϰ� �� �����Ӹ��� ������ �Ǵµ�
        Vec4 dir{ look.x, look.y, look.z, 0.0f };
        razerParticleSystem->SetEmitDirection(dir);
        razerParticle->AddComponent(razerParticleSystem);

        activeScene->AddGameObject(razerParticle);
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
        obj->GetTransform()->SetLocalPosition(Vec3(-400.f + (i * 400), -800.f, 1.f));
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

// ���� �� UI
#pragma region UI
    {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
        obj->AddComponent(make_shared<Transform>());
        obj->SetName(L"OccupationUI");
        obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
        obj->GetTransform()->SetLocalPosition(Vec3(WINDOWHEIGHT / 2, WINDOWWIDTH / 2, 0.f));
        shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
        {
            shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
            meshRenderer->SetMesh(mesh);
        }
        {
            shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");

            shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Crosshair", L"..\\Resources\\Texture\\Crosshair.png");

            shared_ptr<Material> material = make_shared<Material>();
            material->SetShader(shader);
            material->SetTexture(0, texture);
            meshRenderer->SetMaterial(material);
        }
        obj->AddComponent(meshRenderer);
        activeScene->AddGameObject(obj);
    }
#pragma endregion

// ����
#pragma region Temp
    /*
    shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Temp\\Temp.fbx");

    vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

    for (auto gameObject : gameObjects)
    {
        gameObject->SetName(L"Temppp");
        gameObject->SetCheckFrustum(false);
        gameObject->AddComponent(make_shared<Transform>());
        gameObject->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
        gameObject->GetTransform()->SetLocalPosition(Vec3(0.0f, 300.0f, 0.0f));
        gameObject->GetTransform()->SetLocalRotation(Vec3(-1.5708f, 0.0f, 0.0f));
        activeScene->AddGameObject(gameObject);
    }
    */
#pragma endregion

#pragma region OtherPlayer
    {
        std::array<shared_ptr<GameObject>, 2> obj{};
        for (int i = 0; i < 2; ++i) {
            obj[i] = make_shared<GameObject>();
            obj[i]->SetName(L"OtherPlayer" + std::to_wstring(i + 1));
            // �������� �ø� ����
            obj[i]->SetCheckFrustum(true);
            // ����, ���� ����
            obj[i]->SetStatic(false);

            // 2. Transform ������Ʈ �߰� �� ����
            obj[i]->AddComponent(make_shared<Transform>());
            obj[i]->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
            obj[i]->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
            obj[i]->AddComponent(make_shared<TestOtherPlayerScript>());

            // 3. Collider ����
            obj[i]->AddComponent(make_shared<SphereCollider>());
            dynamic_pointer_cast<SphereCollider>(obj[i]->GetCollider())->SetRadius(0.5f);
            dynamic_pointer_cast<SphereCollider>(obj[i]->GetCollider())->SetCenter(Vec3(0.f, 0.f, 0.f));

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
            obj[i]->AddComponent(meshRenderer);

            // 5. Scene�� �߰�
            activeScene->AddGameObject(obj[i]);
        }
    }
#pragma endregion

    LoadMyParticle();
    LoadDebugParticle();
}

ParticleScene::~ParticleScene() {}

void ParticleScene::LoadMyParticle()
{
// ���� ��ƼŬ
#pragma region IceParticle
    {
        // ��ƼŬ ������Ʈ ����
        shared_ptr<GameObject> iceParticle = make_shared<GameObject>();
        wstring iceParticleName = L"IceParticle";
        iceParticle->SetName(iceParticleName);
        iceParticle->SetCheckFrustum(true);
        iceParticle->SetStatic(false);

        // ��ǥ ������Ʈ �߰�
        iceParticle->AddComponent(make_shared<Transform>());
        iceParticle->GetTransform()->SetLocalPosition(Vec3(400.f, 100.f, -400.f));
        iceParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

        // ��ƼŬ �ý��� ������Ʈ �߰�
        shared_ptr<IceParticleSystem> iceParticleSystem = make_shared<IceParticleSystem>();
        iceParticle->AddComponent(make_shared<TestParticleScript>());
        iceParticle->AddComponent(iceParticleSystem);

        activeScene->AddGameObject(iceParticle);
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
        cataclysmParticle->GetTransform()->SetLocalPosition(Vec3(0.f, -100.f, -400.f));
        cataclysmParticle->GetTransform()->SetLocalRotation(Vec3(0.0f, 90.0f, 0.0f));
        cataclysmParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

        // ��ƼŬ �ý��� ������Ʈ �߰�
        shared_ptr<CataclysmParticleSystem> fireParticleSystem = make_shared<CataclysmParticleSystem>();
        cataclysmParticle->AddComponent(make_shared<TestParticleScript>());
        cataclysmParticle->AddComponent(fireParticleSystem);

        activeScene->AddGameObject(cataclysmParticle);
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

// ��¦�� ��ƼŬ
#pragma region GlitterParticle
    {
        // ��ƼŬ ������Ʈ ����
        shared_ptr<GameObject> gliggerParticle = make_shared<GameObject>();
        wstring glitterParticleName = L"GlitterParticle";
        gliggerParticle->SetName(glitterParticleName);
        gliggerParticle->SetCheckFrustum(true);
        gliggerParticle->SetStatic(false);

        // ��ǥ ������Ʈ �߰�
        gliggerParticle->AddComponent(make_shared<Transform>());
        gliggerParticle->GetTransform()->SetLocalPosition(Vec3(400.f, -100.f, -400.f));
        gliggerParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

        // ��ƼŬ �ý��� ������Ʈ �߰�
        shared_ptr<GlitterParticleSystem> gliggerParticleSystem = make_shared<GlitterParticleSystem>();
        gliggerParticle->AddComponent(make_shared<TestParticleScript>());
        gliggerParticle->AddComponent(gliggerParticleSystem);

        //activeScene->AddGameObject(gliggerParticle);
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
        testPBRParticle->GetTransform()->SetLocalPosition(Vec3(0.f, 500.f, 400.f));
        testPBRParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

        // ��ƼŬ �ý��� ������Ʈ �߰�
        shared_ptr<TestPBRParticleSystem> testPBRParticleSystem = make_shared<TestPBRParticleSystem>();
        testPBRParticle->AddComponent(make_shared<TestParticleScript>());
        testPBRParticle->AddComponent(testPBRParticleSystem);

        activeScene->AddGameObject(testPBRParticle);
    }
#pragma endregion
}
