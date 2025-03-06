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
	// 여기서 받아서 위치 계산 후 

	Vec3 pos = GetTransform()->GetLocalPosition();

	if (INPUT->GetButton(KEY_TYPE::UP))
		pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::DOWN))
		pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::LEFT))
		pos -= Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::RIGHT))
		pos += Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::PAGEUP))
		pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::PAGEDOWN))
		pos -= Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;

	if (INPUT->GetButtonDown(KEY_TYPE::T)) PRINTPOSITION;

	// 맵 크기 제한
	float mapMinX = -4900.f;
	float mapMaxX = 4900.f;
	float mapMinZ = -4900.f;
	float mapMaxZ = 4900.f;
	float minY = 500.f;
	float maxY = 9500.f;

	// X, Y, Z 좌표를 맵 범위로 제한
	pos.x = max(mapMinX, min(pos.x, mapMaxX));
	pos.y = max(minY, min(pos.y, maxY));
	pos.z = max(mapMinZ, min(pos.z, mapMaxZ));

	GetTransform()->SetLocalPosition(pos);
}

void TestPointLightScript::MouseInput()
{
	
}