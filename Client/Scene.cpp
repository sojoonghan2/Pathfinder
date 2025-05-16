#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "GameFramework.h"
#include "Buffer.h"
#include "Light.h"
#include "Resources.h"

void Scene::Awake()
{
	for (const auto& gameObject : _gameObjects)
		gameObject->Awake();
}

void Scene::Start()
{
	for (const auto& gameObject : _gameObjects)
		gameObject->Start();
}

void Scene::Update()
{
	for (const auto& gameObject : _gameObjects)
		gameObject->Update();
}

void Scene::LateUpdate()
{
	for (const auto& gameObject : _gameObjects)
		gameObject->LateUpdate();
}

void Scene::FinalUpdate()
{
	for (const auto& gameObject : _gameObjects)
		gameObject->FinalUpdate();
}

void Scene::Init() {}

shared_ptr<Camera> Scene::GetMainCamera()
{
	return _cameras.empty() ? nullptr : _cameras[0];
}

void Scene::Render()
{
	PushLightData();
	ClearRTV();
	RenderShadow();
	RenderDeferred();
	RenderLights();
	RenderFinal();
	RenderForward();
}

void Scene::ClearRTV()
{
	auto swapChain = GFramework->GetSwapChain();
	int8 backIndex = swapChain->GetBackBufferIndex();
	GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);
	GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();
	GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();
	GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();
}

void Scene::RenderShadow()
{
	auto rtGroup = GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW);
	rtGroup->OMSetRenderTargets();

	for (auto& light : _lights)
	{
		if (light->GetLightType() == LIGHT_TYPE::DIRECTIONAL_LIGHT)
			light->RenderShadow();
	}

	rtGroup->WaitTargetToResource();
}

void Scene::RenderDeferred()
{
	auto mainCamera = GetMainCamera();
	if (!mainCamera) return;

	auto rtGroup = GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER);
	rtGroup->OMSetRenderTargets();

	mainCamera->SortGameObject();
	mainCamera->Render_Deferred();

	rtGroup->WaitTargetToResource();
}

void Scene::RenderLights()
{
	auto mainCamera = GetMainCamera();
	if (!mainCamera) return;

	Camera::S_MatView = mainCamera->GetViewMatrix();
	Camera::S_MatProjection = mainCamera->GetProjectionMatrix();

	auto rtGroup = GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING);
	rtGroup->OMSetRenderTargets();

	for (auto& light : _lights)
		light->Render();

	rtGroup->WaitTargetToResource();
}

void Scene::RenderFinal()
{
	int8 backIndex = GFramework->GetSwapChain()->GetBackBufferIndex();
	auto swapRT = GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN);
	swapRT->OMSetRenderTargets(1, backIndex);

	auto finalMat = GET_SINGLE(Resources)->Get<Material>(L"Final");
	finalMat->PushGraphicsData();
	GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render();

	// 결과 복사
	{
		auto renderTex = swapRT->GetRTTexture(backIndex);
		auto copied = GET_SINGLE(Resources)->CloneRenderTargetTexture(renderTex);
		GET_SINGLE(Resources)->SetRenderTargetTexture(copied);
	}

	{
		auto colorTex = GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(2);
		auto copied = GET_SINGLE(Resources)->CloneRenderTargetTexture(colorTex);
		GET_SINGLE(Resources)->SetColorTexture(copied);
	}
}

void Scene::RenderForward()
{
	auto mainCamera = GetMainCamera();
	if (!mainCamera) return;

	mainCamera->Render_Forward();

	for (const auto& cam : _cameras)
	{
		if (cam == mainCamera) continue;
		cam->SortGameObject();
		cam->Render_Forward();
	}
}

void Scene::PushLightData()
{
	LightParams lightParams = {};

	for (auto& light : _lights)
	{
		const LightInfo& info = light->GetLightInfo();
		light->SetLightIndex(lightParams.lightCount);
		lightParams.lights[lightParams.lightCount++] = info;
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	if (auto cam = gameObject->GetCamera())
		_cameras.push_back(cam);
	else if (auto light = gameObject->GetLight())
		_lights.push_back(light);

	_gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	if (auto cam = gameObject->GetCamera())
		std::erase(_cameras, cam);
	else if (auto light = gameObject->GetLight())
		std::erase(_lights, light);

	std::erase(_gameObjects, gameObject);
}