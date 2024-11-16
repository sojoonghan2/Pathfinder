#include "pch.h"
#include "TestPointLightScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

TestPointLightScript::TestPointLightScript()
{
}

TestPointLightScript::~TestPointLightScript()
{
}

void TestPointLightScript::LateUpdate()
{
	KeyboardInput();
	MouseInput();
}

void TestPointLightScript::KeyboardInput()
{
	Vec3 pos = GetTransform()->GetLocalPosition();

	if (INPUT->GetButton(KEY_TYPE::UP))
		pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::DOWN))
		pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::LEFT))
		pos -= Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::RIGHT))
		pos += Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

	if (INPUT->GetButtonDown(KEY_TYPE::T)) PRINTPOSITION;

	GetTransform()->SetLocalPosition(pos);
}

void TestPointLightScript::MouseInput()
{
	
}