#include "pch.h"
#include "Player.h"
#include "Engine.h"
#include "Camera.h"
#include "GameObject.h"
#include "TestGrenadeScript.h"
#include "Resources.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "FireParticleSystem.h"
#include "RazerParticleSystem.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TestParticleScript.h"

Player::Player() : Component(COMPONENT_TYPE::PLAYER)
{

}

Player::~Player()
{

}

void Player::FinalUpdate()
{
    if (!_isInit)
    {
        //LoadPlayerSkill();
        _isInit = true;
    }
}

void Player::LoadPlayerSkill()
{
    LoadGrenade();
    LoadRazer();
}

void Player::LoadGrenade()
{
    shared_ptr<GameObject> grenade = make_shared<GameObject>();
    grenade->SetName(L"Grenade");
    grenade->SetCheckFrustum(true);
    grenade->SetStatic(false);

    grenade->AddComponent(make_shared<Transform>());
    grenade->GetTransform()->SetLocalScale(Vec3(30.f, 30.f, 30.f));
    grenade->GetTransform()->SetParent(GetGameObject()->GetTransform());
    grenade->GetTransform()->GetTransform()->RemoveParent();
    grenade->GetTransform()->SetLocalPosition(Vec3(0.f, 100000000000.f, 0.f));
    grenade->AddComponent(make_shared<TestGrenadeScript>());

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

    GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(grenade);
}

void Player::LoadRazer()
{
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
        razerParticle->GetTransform()->SetParent(GetGameObject()->GetTransform());
        razerParticle->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
        razerParticle->GetTransform()->SetLocalPosition(Vec3(0.0f, 100.0f, 110.0f));
        razerParticle->AddComponent(make_shared<TestParticleScript>());

        // 파티클 시스템 컴포넌트 추가
        shared_ptr<RazerParticleSystem> razerParticleSystem = make_shared<RazerParticleSystem>();
        Vec3 look = GetGameObject()->GetTransform()->GetLook();
        look.Normalize();
        razerParticleSystem->SetEmitDirection(look);
        razerParticle->AddComponent(razerParticleSystem);

        GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(razerParticle);
    }
#pragma endregion
}
