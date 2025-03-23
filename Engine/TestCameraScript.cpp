#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"

TestCameraScript::TestCameraScript()
{
    _offsetPosition = Vec3(0.f, 500.f, -600.f);
}

TestCameraScript::~TestCameraScript()
{
}

void TestCameraScript::LateUpdate()
{
	KeyboardInput();
	MouseInput();

    if (_playerCamera)
    {
        // 타겟이 될 플레이어 오브젝트를 찾음
        _target = GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ");

        // 타겟의 월드 position을 가져와서 offset을 더함
        Vec3 targetPos = _target->GetTransform()->GetLocalPosition();
        GetTransform()->SetLocalPosition(targetPos + _offsetPosition);
    }
}

void TestCameraScript::KeyboardInput()
{
    Vec3 pos = GetTransform()->GetLocalPosition();

    if (!_playerCamera)
    {
        // 이동 처리
        if (INPUT->GetButton(KEY_TYPE::UP)) 
            pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::DOWN))
            pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::LEFT))
            pos -= Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::RIGHT))
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
    //pos.x = max(mapMinX, min(pos.x, mapMaxX));
    //pos.y = max(minY, min(pos.y, maxY));
    //pos.z = max(mapMinZ, min(pos.z, mapMaxZ));

    // 디버깅용 좌표 출력
    if (INPUT->GetButtonDown(KEY_TYPE::T))
        PRINTPOSITION;

    if (INPUT->GetButtonDown(KEY_TYPE::TAB))
    {
        ToggleCamera();
        std::cout << "Change Camera: " << _playerCamera << "\n";
    }

    GetTransform()->SetLocalPosition(pos);
}

void TestCameraScript::MouseInput()
{
    // 플레이어 카메라 모드
    if (_playerCamera)
    {
        Vec3 initialRotation = Vec3(_tempxRotation, _tempyRotation, 0.f);
        GetTransform()->SetLocalRotation(initialRotation);
        return;
    }

    // 개발자 카메라 모드
    if (INPUT->GetButton(KEY_TYPE::LBUTTON))
    {
        POINT mouseDelta = INPUT->GetMouseDelta();
        Vec3 rotation = GetTransform()->GetLocalRotation();
        rotation.y += mouseDelta.x * 0.005f;
        rotation.x += mouseDelta.y * 0.005f;
        GetTransform()->SetLocalRotation(rotation);
    }

    if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
    {
        const POINT& pos2 = INPUT->GetMousePos();
        GET_SINGLE(SceneManager)->Pick(pos2.x, pos2.y);
    }
}

void TestCameraScript::ToggleCamera()
{
    _playerCamera = !_playerCamera;
}