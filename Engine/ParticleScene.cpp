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
// 컴퓨트 셰이더, 멀티쓰레드로 작업이 가능
#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV 용 Texture 생성
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// 쓰레드 그룹 (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

// 카메라
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, FOV=45도
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->GetCamera()->SetFar(10000.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
		activeScene->AddGameObject(camera);
	}
#pragma endregion

// UI 카메라
#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
		activeScene->AddGameObject(camera);
	}
#pragma endregion

// 스카이 박스
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

// 터레인 큐브
#pragma region TerrainCube
	{
		// 1. 기본 오브젝트 생성 및 설정
		shared_ptr<GameObject> terraincube = make_shared<GameObject>();
		terraincube->AddComponent(make_shared<Transform>());
		terraincube->SetCheckFrustum(true);

		// 2. Transform 컴포넌트 추가 및 설정
		terraincube->AddComponent(make_shared<Transform>());
		// 씬의 임시 크기
		terraincube->GetTransform()->SetLocalScale(Vec3(2000.f, 2000.f, 2000.f));
		// 씬의 임시 좌표
		terraincube->GetTransform()->SetLocalPosition(Vec3(0, 800.f, 0.f));

		// 3. MeshRenderer 설정
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

		// 4. Scene에 추가
		activeScene->AddGameObject(terraincube);
	}
#pragma endregion

// 전역 조명
#pragma region Directional Light
	{
		// 1. light 오브젝트 생성 
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Directional_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

		// 2-1. light 컴포넌트 추가 및 속성 설정
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);

		// 2-2. DIRECTIONAL_LIGHT의 경우 조명 방향 설정
		light->GetLight()->SetLightDirection(Vec3(0.f, 0.f, 0.f));

		// 3. 조명 색상 및 강도 설정
		light->GetLight()->SetDiffuse(Vec3(0.8f, 0.8f, 0.8f));
		light->GetLight()->SetAmbient(Vec3(0.8f, 0.8f, 0.8f));
		light->GetLight()->SetSpecular(Vec3(0.05f, 0.05f, 0.05f));

		// 4. Scene에 추가
		activeScene->AddGameObject(light);
	}
#pragma endregion

// 구 오브젝트과 포인트 조명
#pragma region Object & Point Light
	{
		// 1. 기본 오브젝트 생성 및 설정
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetName(L"OBJ");
		// 프러스텀 컬링 여부
		obj->SetCheckFrustum(true);
		// 정적, 동적 여부
		obj->SetStatic(false);

		// 2. Transform 컴포넌트 추가 및 설정
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0, 0.f, 0.f));
		obj->AddComponent(make_shared<TestPointLightScript>());

		// 3. Collider 설정
		obj->AddComponent(make_shared<SphereCollider>());
		dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetRadius(0.5f);
		dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetCenter(Vec3(0.f, 0.f, 0.f));

		// 4. MeshRenderer 설정
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			// GameObject 이름을 가진 머터리얼을 찾아 클론 후 메쉬 렌더러에 Set
			// Resources.cpp로 가면 셰이더와 머터리얼의 생성을 확이해볼 수 있음
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
			meshRenderer->SetMaterial(material->Clone());
		}
		obj->AddComponent(meshRenderer);

		// 5. Scene에 추가
		activeScene->AddGameObject(obj);

		// 1. light 오브젝트 생성 
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Point_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0, 0.f, 0.f));

		// 2-1. light 컴포넌트 추가 및 속성 설정
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::POINT_LIGHT);
		light->AddComponent(make_shared<TestPointLightScript>());

		// 2-2. 점광원 특수 설정
		light->GetLight()->SetLightRange(1000.f);

		// 3. 조명 색상 및 강도 설정
		light->GetLight()->SetDiffuse(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetAmbient(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetSpecular(Vec3(0.5f, 0.5f, 0.5f));

		// 4. Scene에 추가
		activeScene->AddGameObject(light);
	}
#pragma endregion

	if (PARTICLEDEBUG) LoadDebugParticle();
	else LoadMyParticle();
}

ParticleScene::~ParticleScene() {}

void ParticleScene::LoadMyParticle()
{
// 얼음 파티클
#pragma region IceParticle
	// 테스트 결과 100개를 만들면 fps 20까지 떨어짐
	// VRS 테스트 시 파티클 100개 띄우면 될듯
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> iceParticle = make_shared<GameObject>();
		wstring iceParticleName = L"IceParticle";
		iceParticle->SetName(iceParticleName);
		iceParticle->SetCheckFrustum(true);
		iceParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		iceParticle->AddComponent(make_shared<Transform>());
		iceParticle->GetTransform()->SetLocalPosition(Vec3(-400.f, 100.f, -400.f));
		iceParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<IceParticleSystem> iceParticleSystem = make_shared<IceParticleSystem>();
		iceParticle->AddComponent(make_shared<TestParticleScript>());
		iceParticle->AddComponent(iceParticleSystem);

		activeScene->AddGameObject(iceParticle);
	}
#pragma endregion

// 불 파티클
#pragma region FireParticle
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> fireParticle = make_shared<GameObject>();
		wstring fireParticleName = L"FireParticle";
		fireParticle->SetName(fireParticleName);
		fireParticle->SetCheckFrustum(true);
		fireParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		fireParticle->AddComponent(make_shared<Transform>());
		fireParticle->GetTransform()->SetLocalPosition(Vec3(400.f, 100.f, -400.f));
		fireParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<FireParticleSystem> fireParticleSystem = make_shared<FireParticleSystem>();
		fireParticle->AddComponent(make_shared<TestParticleScript>());
		fireParticle->AddComponent(fireParticleSystem);

		activeScene->AddGameObject(fireParticle);
	}
#pragma endregion

// 대격변 파티클
#pragma region CataclysmParticle
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> cataclysmParticle = make_shared<GameObject>();
		wstring cataclysmParticleName = L"CataclysmParticle";
		cataclysmParticle->SetName(cataclysmParticleName);
		cataclysmParticle->SetCheckFrustum(true);
		cataclysmParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		cataclysmParticle->AddComponent(make_shared<Transform>());
		cataclysmParticle->GetTransform()->SetLocalPosition(Vec3(0.f, -100.f, 400.f));
		cataclysmParticle->GetTransform()->SetLocalRotation(Vec3(0.0f, 90.0f, 0.0f));
		cataclysmParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<CataclysmParticleSystem> fireParticleSystem = make_shared<CataclysmParticleSystem>();
		cataclysmParticle->AddComponent(make_shared<TestParticleScript>());
		cataclysmParticle->AddComponent(fireParticleSystem);

		activeScene->AddGameObject(cataclysmParticle);
	}
#pragma endregion

// 레이저 파티클
#pragma region RazerParticle
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> razerParticle = make_shared<GameObject>();
		wstring razerParticleName = L"RazerParticle";
		razerParticle->SetName(razerParticleName);
		razerParticle->SetCheckFrustum(true);
		razerParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		razerParticle->AddComponent(make_shared<Transform>());
		razerParticle->GetTransform()->SetLocalPosition(Vec3(-400.f, 100.f, 400.f));
		razerParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<RazerParticleSystem> razerParticleSystem = make_shared<RazerParticleSystem>();
		razerParticle->AddComponent(make_shared<TestParticleScript>());
		razerParticle->AddComponent(razerParticleSystem);

		activeScene->AddGameObject(razerParticle);
	}
#pragma endregion

// 오버 드라이브 파티클
#pragma region OverDriveParticle
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> overDriveParticle = make_shared<GameObject>();
		wstring overDriveParticleName = L"OverDriveParticle";
		overDriveParticle->SetName(overDriveParticleName);
		overDriveParticle->SetCheckFrustum(true);
		overDriveParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		overDriveParticle->AddComponent(make_shared<Transform>());
		overDriveParticle->GetTransform()->SetLocalPosition(Vec3(400.f, -100.f, 400.f));
		overDriveParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<OverDriveParticleSystem> overDriveParticleSystem = make_shared<OverDriveParticleSystem>();
		overDriveParticle->AddComponent(make_shared<TestParticleScript>());
		overDriveParticle->AddComponent(overDriveParticleSystem);

		activeScene->AddGameObject(overDriveParticle);
	}
#pragma endregion
}

void ParticleScene::LoadDebugParticle()
{
// 테스트 PBR 파티클
#pragma region TestPBRParticle
	{
		// 파티클 오브젝트 생성
		shared_ptr<GameObject> testPBRParticle = make_shared<GameObject>();
		wstring testPBRParticleName = L"testPBRParticle";
		testPBRParticle->SetName(testPBRParticleName);
		testPBRParticle->SetCheckFrustum(true);
		testPBRParticle->SetStatic(false);

		// 좌표 컴포넌트 추가
		testPBRParticle->AddComponent(make_shared<Transform>());
		testPBRParticle->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 400.f));
		testPBRParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// 파티클 시스템 컴포넌트 추가
		shared_ptr<TestPBRParticleSystem> testPBRParticleSystem = make_shared<TestPBRParticleSystem>();
		testPBRParticle->AddComponent(make_shared<TestParticleScript>());
		testPBRParticle->AddComponent(testPBRParticleSystem);

		activeScene->AddGameObject(testPBRParticle);
	}
#pragma endregion
}
