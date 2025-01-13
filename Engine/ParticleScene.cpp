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

#include "TestDragon.h"
#include "TestPointLightScript.h"
#include "WaterScript.h"
#include "TestParticleScript.h"

#include "SphereCollider.h"

ParticleScene::ParticleScene()
{
// ÄÄÇ»Æ® ¼ÎÀÌ´õ, ¸ÖÆ¼¾²·¹µå·Î ÀÛ¾÷ÀÌ °¡´É
#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV ¿ë Texture »ı¼º
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// ¾²·¹µå ±×·ì (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

// Ä«¸Ş¶ó
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, FOV=45µµ
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->GetCamera()->SetFar(10000.f);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI´Â ¾È ÂïÀ½
		activeScene->AddGameObject(camera);
	}
#pragma endregion

// UI Ä«¸Ş¶ó
#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=3000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // ´Ù ²ô°í
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI¸¸ ÂïÀ½
		activeScene->AddGameObject(camera);
	}
#pragma endregion

// ½ºÄ«ÀÌ ¹Ú½º
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

// ÅÍ·¹ÀÎ Å¥ºê
#pragma region TerrainCube
	{
		// 1. ±âº» ¿ÀºêÁ§Æ® »ı¼º ¹× ¼³Á¤
		shared_ptr<GameObject> terraincube = make_shared<GameObject>();
		terraincube->AddComponent(make_shared<Transform>());
		terraincube->SetCheckFrustum(true);

		// 2. Transform ÄÄÆ÷³ÍÆ® Ãß°¡ ¹× ¼³Á¤
		terraincube->AddComponent(make_shared<Transform>());
		// ¾ÀÀÇ ÀÓ½Ã Å©±â
		terraincube->GetTransform()->SetLocalScale(Vec3(2000.f, 2000.f, 2000.f));
		// ¾ÀÀÇ ÀÓ½Ã ÁÂÇ¥
		terraincube->GetTransform()->SetLocalPosition(Vec3(0, 800.f, 0.f));

		// 3. MeshRenderer ¼³Á¤
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

		// 4. Scene¿¡ Ãß°¡
		activeScene->AddGameObject(terraincube);
	}
#pragma endregion

// Àü¿ª Á¶¸í
#pragma region Directional Light
	{
		// 1. light ¿ÀºêÁ§Æ® »ı¼º 
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->SetName(L"Directional_Light");
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

		// 2-1. light ÄÄÆ÷³ÍÆ® Ãß°¡ ¹× ¼Ó¼º ¼³Á¤
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);

		// 2-2. DIRECTIONAL_LIGHTÀÇ °æ¿ì Á¶¸í ¹æÇâ ¼³Á¤
		light->GetLight()->SetLightDirection(Vec3(0.f, 0.f, 0.f));

		// 3. Á¶¸í »ö»ó ¹× °­µµ ¼³Á¤
		light->GetLight()->SetDiffuse(Vec3(0.8f, 0.8f, 0.8f));
		light->GetLight()->SetAmbient(Vec3(0.8f, 0.8f, 0.8f));
		light->GetLight()->SetSpecular(Vec3(0.05f, 0.05f, 0.05f));

		// 4. Scene¿¡ Ãß°¡
		activeScene->AddGameObject(light);
	}
#pragma endregion

// ¾óÀ½ ÆÄÆ¼Å¬
#pragma region IceParticle
	// Å×½ºÆ® °á°ú 100°³¸¦ ¸¸µé¸é fps 20±îÁö ¶³¾îÁü
	// VRS Å×½ºÆ® ½Ã ÆÄÆ¼Å¬ 100°³ ¶ç¿ì¸é µÉµí
	{
		// ÆÄÆ¼Å¬ ¿ÀºêÁ§Æ® »ı¼º
		shared_ptr<GameObject> iceParticle = make_shared<GameObject>();
		wstring iceParticleName = L"IceParticle";
		iceParticle->SetName(iceParticleName);
		iceParticle->SetCheckFrustum(true);
		iceParticle->SetStatic(false);

		// ÁÂÇ¥ ÄÄÆ÷³ÍÆ® Ãß°¡
		iceParticle->AddComponent(make_shared<Transform>());
		iceParticle->GetTransform()->SetLocalPosition(Vec3(-200.f, 100.f, -200.f));
		iceParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ÆÄÆ¼Å¬ ½Ã½ºÅÛ ÄÄÆ÷³ÍÆ® Ãß°¡
		shared_ptr<IceParticleSystem> iceParticleSystem = make_shared<IceParticleSystem>();
		iceParticle->AddComponent(make_shared<TestParticleScript>());
		iceParticle->AddComponent(iceParticleSystem);

		activeScene->AddGameObject(iceParticle);
	}
#pragma endregion

// ºÒ ÆÄÆ¼Å¬
#pragma region FireParticle
	{
		// ÆÄÆ¼Å¬ ¿ÀºêÁ§Æ® »ı¼º
		shared_ptr<GameObject> fireParticle = make_shared<GameObject>();
		wstring fireParticleName = L"FireParticle";
		fireParticle->SetName(fireParticleName);
		fireParticle->SetCheckFrustum(true);
		fireParticle->SetStatic(false);

		// ÁÂÇ¥ ÄÄÆ÷³ÍÆ® Ãß°¡
		fireParticle->AddComponent(make_shared<Transform>());
		fireParticle->GetTransform()->SetLocalPosition(Vec3(200.f, 100.f, 200.f));
		fireParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ÆÄÆ¼Å¬ ½Ã½ºÅÛ ÄÄÆ÷³ÍÆ® Ãß°¡
		shared_ptr<FireParticleSystem> fireParticleSystem = make_shared<FireParticleSystem>();
		fireParticle->AddComponent(make_shared<TestParticleScript>());
		fireParticle->AddComponent(fireParticleSystem);

		activeScene->AddGameObject(fireParticle);
	}
#pragma endregion
<<<<<<< HEAD

// ´ë°İº¯ ÆÄÆ¼Å¬
#pragma region CataclysmParticle
	{
		// ÆÄÆ¼Å¬ ¿ÀºêÁ§Æ® »ı¼º
		shared_ptr<GameObject> cataclysmParticle = make_shared<GameObject>();
		wstring cataclysmParticleName = L"CataclysmParticle";
		cataclysmParticle->SetName(cataclysmParticleName);
		cataclysmParticle->SetCheckFrustum(true);
		cataclysmParticle->SetStatic(false);

		// ÁÂÇ¥ ÄÄÆ÷³ÍÆ® Ãß°¡
		cataclysmParticle->AddComponent(make_shared<Transform>());
		cataclysmParticle->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 200.f));
		cataclysmParticle->GetTransform()->SetLocalRotation(Vec3(0.0f, 90.0f, 0.0f));
		cataclysmParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ÆÄÆ¼Å¬ ½Ã½ºÅÛ ÄÄÆ÷³ÍÆ® Ãß°¡
		shared_ptr<CataclysmParticleSystem> fireParticleSystem = make_shared<CataclysmParticleSystem>();
		cataclysmParticle->AddComponent(make_shared<TestParticleScript>());
		cataclysmParticle->AddComponent(fireParticleSystem);

		activeScene->AddGameObject(cataclysmParticle);
	}
#pragma endregion

// ºÒ ÆÄÆ¼Å¬
#pragma region FireParticle
	{
		// ÆÄÆ¼Å¬ ¿ÀºêÁ§Æ® »ı¼º
		shared_ptr<GameObject> fireParticle = make_shared<GameObject>();
		wstring fireParticleName = L"FireParticle";
		fireParticle->SetName(fireParticleName);
		fireParticle->SetCheckFrustum(true);
		fireParticle->SetStatic(false);

		// ÁÂÇ¥ ÄÄÆ÷³ÍÆ® Ãß°¡
		fireParticle->AddComponent(make_shared<Transform>());
		fireParticle->GetTransform()->SetLocalPosition(Vec3(200.f, 100.f, 200.f));
		fireParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ÆÄÆ¼Å¬ ½Ã½ºÅÛ ÄÄÆ÷³ÍÆ® Ãß°¡
		shared_ptr<FireParticleSystem> fireParticleSystem = make_shared<FireParticleSystem>();
		fireParticle->AddComponent(make_shared<TestParticleScript>());
		fireParticle->AddComponent(fireParticleSystem);

		activeScene->AddGameObject(fireParticle);
	}
#pragma endregion

// ·¹ÀÌÀú ÆÄÆ¼Å¬
#pragma region RazerParticle
	{
		// ÆÄÆ¼Å¬ ¿ÀºêÁ§Æ® »ı¼º
		shared_ptr<GameObject> razerParticle = make_shared<GameObject>();
		wstring razerParticleName = L"RazerParticle";
		razerParticle->SetName(razerParticleName);
		razerParticle->SetCheckFrustum(true);
		razerParticle->SetStatic(false);

		// ÁÂÇ¥ ÄÄÆ÷³ÍÆ® Ãß°¡
		razerParticle->AddComponent(make_shared<Transform>());
		razerParticle->GetTransform()->SetLocalPosition(Vec3(-400.f, 100.f, 200.f));
		razerParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));

		// ÆÄÆ¼Å¬ ½Ã½ºÅÛ ÄÄÆ÷³ÍÆ® Ãß°¡
		shared_ptr<FireParticleSystem> razerParticleSystem = make_shared<FireParticleSystem>();
		razerParticle->AddComponent(make_shared<TestParticleScript>());
		razerParticle->AddComponent(razerParticleSystem);

		activeScene->AddGameObject(razerParticle);
	}
#pragma endregion
=======
>>>>>>> parent of cbe179b (ëŒ€ê²©ë³€ íŒŒí‹°í´ ì¶”ê°€)
}

ParticleScene::~ParticleScene() {}