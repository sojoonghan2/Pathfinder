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

    if (!_playerCamera)
    {
        // 이동 처리
        if (INPUT->GetButton(KEY_TYPE::W))
            pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::S))
            pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::A))
            pos -= Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::D))
            pos += Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;
    }

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

    // 디버깅용 좌표 출력
    if (INPUT->GetButtonDown(KEY_TYPE::T))
        PRINTPOSITION;

    if (INPUT->GetButtonDown(KEY_TYPE::TAB))
    {
        ToggleCamera();
        std::cout << _playerCamera << "\n";
    }

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

// TODO: 플레이어 카메라와 개발자 카메라를 토글로 분리, 수정 필요
void TestCameraScript::ToggleCamera()
{
    _playerCamera = !_playerCamera;

    if (_playerCamera)
    {
        GetTransform()->RestoreParent();
        GetTransform()->SetLocalPosition(Vec3(0.f, 2.f, 0.f));
        GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
    }
    else
    {
        GetTransform()->RemoveParent();
        GetTransform()->SetLocalPosition(Vec3(0.f, 5.f, -10.f));
        GetTransform()->SetLocalRotation(Vec3(0.2f, 0.f, 0.f));
    }
}