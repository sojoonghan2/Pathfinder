#include "pch.h"
#include "TestParticleScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "ParticleSystem.h"

TestParticleScript::TestParticleScript()
{
}

TestParticleScript::~TestParticleScript()
{
}

void TestParticleScript::LateUpdate()
{
	KeyboardInput();
	MouseInput();
}

void TestParticleScript::KeyboardInput()
{

	if (INPUT->GetButton(KEY_TYPE::U))
	{

	}

}

void TestParticleScript::MouseInput()
{

}