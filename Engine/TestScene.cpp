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
// 카메라가 없으면 오류가 난다.
#pragma region Camera
    {
        shared_ptr<GameObject> camera = make_shared<GameObject>();
        camera->SetName(L"Main_Camera");
        camera->AddComponent(make_shared<Transform>());
        camera->AddComponent(make_shared<Camera>());
        // 카메라를 움직일 수 있는 스크립트를 추가
        camera->AddComponent(make_shared<TestCameraScript>());
        camera->GetCamera()->SetFar(10000.f);
        camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
        uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
        camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
        activeScene->AddGameObject(camera);
    }
#pragma endregion

// 임시 사각형 객체
#pragma region UM
    // 1. 임시 오브젝트 생성 및 설정
    shared_ptr<GameObject> um = make_shared<GameObject>();
    um->SetName(L"Um");
    um->SetCheckFrustum(true);

    // 2. Transform 컴포넌트 추가 및 설정
    um->AddComponent(make_shared<Transform>());
    um->GetTransform()->SetLocalScale(Vec3(WINDOWHEIGHT, WINDOWWIDTH, 1.f));
    um->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 1.f));

    // 3. MeshRenderer 설정
    shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
    {
        // 사각형 메쉬를 가져와서
        shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
        // 메쉬렌더러에 사각형 메쉬를 바인딩
        meshRenderer->SetMesh(mesh);
    }
    {
        // Texture라는 이름을 가진 셰이더를 가져오고
        shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");
        // 실제 텍스쳐 이미지를 가져오고
        shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Um", L"..\\Resources\\Texture\\Um.png");
       // 머터리얼을 만들어서
        shared_ptr<Material> material = make_shared<Material>();
        // 머터리얼에 셰이더와 텍스쳐를 바인딩
        material->SetShader(shader);
        material->SetTexture(0, texture);
        // 메쉬 렌더러에 위에 만든 머터리얼을 바인딩
        meshRenderer->SetMaterial(material);
    }
    // 객체에 메쉬 렌더러 바인딩
    um->AddComponent(meshRenderer);

    // 4. Scene에 추가
    activeScene->AddGameObject(um);
#pragma endregion
}

TestScene::~TestScene() {}