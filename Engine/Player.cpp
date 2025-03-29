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
        LoadPlayerSkill();
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
    
}

void Player::LoadRazer()
{
#pragma region RazerParticle
    {
        
    }
#pragma endregion
}
