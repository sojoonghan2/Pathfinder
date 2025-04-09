#include "pch.h"
#include "CameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Engine.h"

// 선형 보간 함수
Vec3 Lerp(const Vec3& a, const Vec3& b, float t)
{
    return a + (b - a) * t;
}

CameraScript::CameraScript() {
    _defaultOffset = Vec3(0.f, 500.f, -600.f);
    _zoomOffset = Vec3(0.f, 600.f, -250.f);
    _currentOffset = _defaultOffset;

    _centerScreenPos.x = GEngine->GetWindow().width / 2;
    _centerScreenPos.y = GEngine->GetWindow().height / 2;

    ClientToScreen(GEngine->GetWindow().hwnd, &_centerScreenPos);
}

CameraScript::~CameraScript() {
}

void CameraScript::LateUpdate() {
    KeyboardInput();
    MouseInput();

    if (_playerCamera) {
        _target = GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ");
        if (_target == nullptr) return;

        Vec3 targetPos = _target->GetTransform()->GetLocalPosition();

        // 오른쪽 마우스 버튼 상태에 따라 오프셋 보간
        Vec3 targetOffset = INPUT->GetButton(KEY_TYPE::RBUTTON) ? _zoomOffset : _defaultOffset;
        _currentOffset = Lerp(_currentOffset, targetOffset, 5.f * DELTA_TIME); // 빠르게 따라가도록 보간

        // 공전 위치 계산
        Vec3 rotatedOffset;
        rotatedOffset.x = sinf(_revolution.y) * _currentOffset.z + _currentOffset.x * cosf(_revolution.y);
        rotatedOffset.z = cosf(_revolution.y) * _currentOffset.z - _currentOffset.x * sinf(_revolution.y);
        rotatedOffset.y = _currentOffset.y;

        Vec3 camPos = targetPos + rotatedOffset;
        GetTransform()->SetLocalPosition(camPos);

        // 회전 계산
        Vec3 lookDirection = Normalization(targetPos - camPos);
        Vec3 finalRotation = _cameraRotation;
        GetTransform()->LookAt(targetPos);
        finalRotation.y = atan2f(lookDirection.x, lookDirection.z);
        GetTransform()->SetLocalRotation(finalRotation);
    }
}

void CameraScript::KeyboardInput() {
    Vec3 pos = GetTransform()->GetLocalPosition();

    if (!_playerCamera) {
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

void CameraScript::MouseInput() {
    if (_playerCamera)
    {
        while (ShowCursor(FALSE) >= 0);
        POINT delta = INPUT->GetMouseDelta();

        const float maxDelta = 20.0f;
        float dx = std::clamp(static_cast<float>(delta.x), -maxDelta, maxDelta);
        float dy = std::clamp(static_cast<float>(delta.y), -maxDelta, maxDelta);

        _revolution.y += dx * 0.005f;
        _cameraRotation.y += dx * 0.005f;

        _cameraRotation.x += dy * 0.005f;
        const float minAngle = -XM_PI / 18.0f; // -30도
        const float maxAngle = XM_PI / 18.0f;  // 30도
        _cameraRotation.x = std::clamp(_cameraRotation.x, minAngle, maxAngle);

        // 커서가 화면 경계를 넘었는지 검사
        POINT cursorPos;
        GetCursorPos(&cursorPos);

        int margin = 2;
        RECT windowRect;
        GetClientRect(GEngine->GetWindow().hwnd, &windowRect);
        ClientToScreen(GEngine->GetWindow().hwnd, reinterpret_cast<LPPOINT>(&windowRect.left));
        ClientToScreen(GEngine->GetWindow().hwnd, reinterpret_cast<LPPOINT>(&windowRect.right));

        if (cursorPos.x <= windowRect.left + margin ||
            cursorPos.x >= windowRect.right - margin ||
            cursorPos.y <= windowRect.top + margin ||
            cursorPos.y >= windowRect.bottom - margin)
        {
            SetCursorPos(_centerScreenPos.x, _centerScreenPos.y);
        }
    }
    else
    {
        while (ShowCursor(TRUE) < 0);
        if (INPUT->GetButton(KEY_TYPE::LBUTTON)) {
            POINT mouseDelta = INPUT->GetMouseDelta();
            Vec3 rotation = GetTransform()->GetLocalRotation();
            rotation.y += mouseDelta.x * 0.005f;
            rotation.x += mouseDelta.y * 0.005f;
            GetTransform()->SetLocalRotation(rotation);
        }
    }
}

void CameraScript::ToggleCamera()
{
    _playerCamera = !_playerCamera;
}
