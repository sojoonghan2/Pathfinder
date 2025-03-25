#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"

TestCameraScript::TestCameraScript() {
    _offsetPosition = Vec3(0.f, 500.f, -600.f);
}

TestCameraScript::~TestCameraScript() {
}

void TestCameraScript::LateUpdate() {
    KeyboardInput();
    MouseInput();

    if (_playerCamera) {
        _target = GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ");
        if (_target == nullptr) return;

        Vec3 targetPos = _target->GetTransform()->GetLocalPosition();

        // 회전된 오프셋 계산
        Vec3 rotatedOffset;
        rotatedOffset.x = sinf(_revolution.y) * _offsetPosition.z;
        rotatedOffset.z = cosf(_revolution.y) * _offsetPosition.z;
        rotatedOffset.y = _offsetPosition.y;

        Vec3 camPos = targetPos + rotatedOffset;
        GetTransform()->SetLocalPosition(camPos);
    }
}

void TestCameraScript::KeyboardInput() {
    Vec3 pos = GetTransform()->GetLocalPosition();

    if (!_playerCamera) {
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

    if (INPUT->GetButtonDown(KEY_TYPE::TAB)) {
        ToggleCamera();
        std::cout << "Change Camera: " << _playerCamera << "\n";
    }

    GetTransform()->SetLocalPosition(pos);
}

void TestCameraScript::MouseInput() {
    if (_playerCamera) {
        if (INPUT->GetButton(KEY_TYPE::LBUTTON)) {
            POINT delta = INPUT->GetMouseDelta();
            _revolution.y += delta.x * 0.005f;
        }
    }
    else {
        if (INPUT->GetButton(KEY_TYPE::LBUTTON)) {
            POINT mouseDelta = INPUT->GetMouseDelta();
            Vec3 rotation = GetTransform()->GetLocalRotation();
            rotation.y += mouseDelta.x * 0.005f;
            rotation.x += mouseDelta.y * 0.005f;
            GetTransform()->SetLocalRotation(rotation);
        }
    }
}

void TestCameraScript::ToggleCamera() {
    _playerCamera = !_playerCamera;
}