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
        if (INPUT->GetButton(KEY_TYPE::LBUTTON))
        {
            POINT mouseDelta = INPUT->GetMouseDelta();

            // 타겟이 존재하는지 확인
            if (_target != nullptr)
            {
                // 현재 오브젝트와 타겟 사이의 상대적인 위치 계산
                Vec3 targetPos = _target->GetTransform()->GetWorldPosition();
                Vec3 currentPos = GetTransform()->GetWorldPosition();
                Vec3 offset = currentPos - targetPos;
                float distance = offset.Length();

                // 레볼루션 각도 업데이트
                Vec3 revolution = GetTransform()->GetLocalRevolution();
                revolution.y += mouseDelta.x * 0.005f;

                // 레볼루션 적용을 위한 새 위치 계산
                float angle = revolution.y;
                Vec3 newPos;
                newPos.x = targetPos.x + distance * sin(angle);
                newPos.z = targetPos.z + distance * cos(angle);
                newPos.y = currentPos.y; // y축은 유지

                // 새 위치로 이동
                GetTransform()->SetLocalPosition(newPos);

                // 타겟을 바라보도록 회전
                Vec3 dirToTarget = targetPos - newPos;
                GetTransform()->LookAt(dirToTarget);

                // 레볼루션 값 저장 (다음 계산에 사용)
                GetTransform()->SetLocalRevolution(revolution);
            }
        }
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
}

void TestCameraScript::ToggleCamera()
{
    _playerCamera = !_playerCamera;
}