#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

TestCameraScript::TestCameraScript()
{
}

TestCameraScript::~TestCameraScript()
{
}

void TestCameraScript::LateUpdate()
{
	KeyboardInput();
	MouseInput();
}

void TestCameraScript::KeyboardInput()
{
	Vec3 pos = GetTransform()->GetLocalPosition();

	if (INPUT->GetButton(KEY_TYPE::W))
		pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::A))
		pos -= Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::D))
		pos += Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

	if (INPUT->GetButtonDown(KEY_TYPE::T)) PRINTPOSITION;

	GetTransform()->SetLocalPosition(pos);
}

void TestCameraScript::MouseInput()
{
	// 마우스 오른쪽 버튼이 눌려 있는지 확인
	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		// 마우스 이동량을 가져옴 (이 값은 프레임마다 갱신됨)
		POINT mouseDelta = INPUT->GetMouseDelta();

		// 현재 로컬 회전값을 가져옴
		Vec3 rotation = GetTransform()->GetLocalRotation();

		// 마우스 X축 이동은 Y축 회전에 영향을 줌 (좌우 회전)
		rotation.y += mouseDelta.x * 0.005f;

		// 마우스 Y축 이동은 X축 회전에 영향을 줌 (상하 회전)
		rotation.x += mouseDelta.y * 0.005f;

		// 회전값을 다시 설정
		GetTransform()->SetLocalRotation(rotation);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		const POINT& pos2 = INPUT->GetMousePos();
		GET_SINGLE(SceneManager)->Pick(pos2.x, pos2.y);
	}
}