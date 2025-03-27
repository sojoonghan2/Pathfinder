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
#include "SceneManager.h"
#include "Scene.h"

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
        LoadPlayerSkill();
        _isInit = true;
    }
}

void Player::LoadPlayerSkill()
{
    LoadGrenade();
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
