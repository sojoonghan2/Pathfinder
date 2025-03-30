#include "pch.h"
#include "RazerParticleScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "ParticleSystem.h"

RazerParticleScript::RazerParticleScript()
{
}

RazerParticleScript::~RazerParticleScript()
{
}

void RazerParticleScript::LateUpdate()
{
	KeyboardInput();
	MouseInput();
}

void RazerParticleScript::KeyboardInput()
{
    auto particle_system = GetGameObject()->GetParticleSystem();
	if (INPUT->GetButton(KEY_TYPE::R))
	{
		particle_system->ParticleStart();
	}
}

void RazerParticleScript::MouseInput()
{

}