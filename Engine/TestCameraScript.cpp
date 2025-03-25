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
        _target = GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ");

        if (_target == nullptr)
            return;

        // 타겟 위치
        Vec3 targetPos = _target->GetTransform()->GetLocalPosition();

        // Y축 회전에 따른 카메라 오프셋 계산
        Vec3 offset;
        offset.x = sinf(_revolution.y) * _offsetPosition.z;
        offset.z = cosf(_revolution.y) * _offsetPosition.z + 600.f;
        offset.y = _offsetPosition.y + 500.f;

        Vec3 camPos = targetPos + offset;
        GetTransform()->SetLocalPosition(camPos);

        // 카메라는 항상 타겟을 바라보도록 회전
        GetTransform()->LookAt(targetPos);
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
        if (INPUT->GetButton(KEY_TYPE::LBUTTON))
        {
            POINT delta = INPUT->GetMouseDelta();
            _revolution.y += delta.x * 0.005f;
        }
    }
    // 개발자 카메라 모드
    else
    {
        if (INPUT->GetButton(KEY_TYPE::LBUTTON))
        {
            POINT mouseDelta = INPUT->GetMouseDelta();
            Vec3 rotation = GetTransform()->GetLocalRotation();
            rotation.y += mouseDelta.x * 0.005f;
            rotation.x += mouseDelta.y * 0.005f;
            GetTransform()->SetLocalRotation(rotation);
        }
    }
}

void TestCameraScript::ToggleCamera()
{
    _playerCamera = !_playerCamera;
}