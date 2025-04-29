#include "pch.h"
#include "LoadingScene.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "Resources.h"
#include "Mesh.h"
#include "MeshRenderer.h"

#include "LoadingScript.h"

LoadingScene::LoadingScene() : stageClear(false)
{
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"TitleCamera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->AddComponent(make_shared<LoadingScript>());
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll();
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI
		activeScene->AddGameObject(camera);
	}
#pragma endregion
#pragma region Loading
	shared_ptr<GameObject> title = make_shared<GameObject>();
	title->SetName(L"Loading");
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
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Loading", L"..\\Resources\\Texture\\Loading.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	title->AddComponent(meshRenderer);
	activeScene->AddGameObject(title);
#pragma endregion

#pragma region LoadingIcon
	{
		shared_ptr<GameObject> icon = make_shared<GameObject>();
		icon->SetName(L"LoadingIcon");
		icon->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		icon->AddComponent(make_shared<Transform>());
		icon->GetTransform()->SetLocalScale(Vec3(450.f, 450.f, 1.f));
		icon->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 1.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"PNG");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"LoadingIcon", L"..\\Resources\\Texture\\LoadingIcon.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		icon->AddComponent(meshRenderer);
		icon->SetRender(false);
		activeScene->AddGameObject(icon);
	}
#pragma endregion

	// 생성자에서 초기화만 수행
	stageClear = false;
}

LoadingScene::~LoadingScene()
{
}