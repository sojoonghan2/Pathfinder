#include "pch.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Shader.h"
#include "Material.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

#include "TitleScript.h"
#include "MasterScript.h"

#include "Resources.h"
#include "SphereCollider.h"
#include "MeshRenderer.h"

TitleScene::TitleScene()
{
#pragma region Camera
    {
        shared_ptr<GameObject> camera = make_shared<GameObject>();
        camera->SetName(L"TitleCamera");
        camera->AddComponent(make_shared<Transform>());
        camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
        camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
        camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
        uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
        camera->GetCamera()->SetCullingMaskAll();
        camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI
        activeScene->AddGameObject(camera);
    }
#pragma endregion

#pragma region Title
    shared_ptr<GameObject> title = make_shared<GameObject>();
    title->SetName(L"Title");
    title->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
    title->AddComponent(make_shared<Transform>());
    title->GetTransform()->SetLocalScale(Vec3(WINDOWHEIGHT, WINDOWWIDTH, 1.f));
    title->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 1.f));
    shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
    {
        shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
        meshRenderer->SetMesh(mesh);
    }
    {
        shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");
        shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Title", L"..\\Resources\\Texture\\Title.jpg");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, texture);
        meshRenderer->SetMaterial(material);
    }
    title->AddComponent(make_shared<TitleScript>());
    title->AddComponent(meshRenderer);
    activeScene->AddGameObject(title);
#pragma endregion
}

TitleScene::~TitleScene() {}