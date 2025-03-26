#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Engine.h"

TestCameraScript::TestCameraScript() {
    _offsetPosition = Vec3(0.f, 500.f, -600.f);

    // â �߾� ��ġ ���
    _centerScreenPos.x = GEngine->GetWindow().width / 2;
    _centerScreenPos.y = GEngine->GetWindow().height / 2;

    // ȭ�� ���� ��ǥ�� ��ȯ
    ClientToScreen(GEngine->GetWindow().hwnd, &_centerScreenPos);
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

        // ���� ������ ���
        Vec3 rotatedOffset;
        rotatedOffset.x = sinf(_revolution.y) * _offsetPosition.z;
        rotatedOffset.z = cosf(_revolution.y) * _offsetPosition.z;
        rotatedOffset.y = _offsetPosition.y;

        Vec3 camPos = targetPos + rotatedOffset;
        GetTransform()->SetLocalPosition(camPos);

        // ī�޶� ȸ�� ���� (���� + Ÿ�� ���� ���)
        Vec3 lookDirection = Normalization(targetPos - camPos);

        // ��ü ȸ�� ���� ����
        Vec3 finalRotation = _cameraRotation;
        GetTransform()->SetLocalPosition(camPos);
        GetTransform()->LookAt(targetPos);
        finalRotation.y = atan2f(lookDirection.x, lookDirection.z);

        GetTransform()->SetLocalRotation(finalRotation);
    }
}

void TestCameraScript::KeyboardInput() {
    Vec3 pos = GetTransform()->GetLocalPosition();

    if (!_playerCamera) {
        // �̵� ó��
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
    if (_playerCamera)
    {
        while (ShowCursor(FALSE) >= 0);
        POINT delta = INPUT->GetMouseDelta();

        // �ִ� ȸ�� �ӵ� ����
        const float maxDelta = 20.0f;
        float dx = std::clamp(static_cast<float>(delta.x), -maxDelta, maxDelta);
        float dy = std::clamp(static_cast<float>(delta.y), -maxDelta, maxDelta);

        _revolution.y += dx * 0.005f;
        _cameraRotation.y += dx * 0.005f;
        // _cameraRotation.x += dy * 0.005f;

        // Ŀ���� ȭ�� ��踦 �Ѿ����� �˻�
        POINT cursorPos;
        GetCursorPos(&cursorPos);

        int margin = 2; // ���� ����
        RECT windowRect;
        GetClientRect(GEngine->GetWindow().hwnd, &windowRect);
        ClientToScreen(GEngine->GetWindow().hwnd, reinterpret_cast<LPPOINT>(&windowRect.left));
        ClientToScreen(GEngine->GetWindow().hwnd, reinterpret_cast<LPPOINT>(&windowRect.right));

        if (cursorPos.x <= windowRect.left + margin ||
            cursorPos.x >= windowRect.right - margin)
        {
            SetCursorPos(_centerScreenPos.x, _centerScreenPos.y);
            //INPUT->ResetMousePosition(_centerScreenPos);
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

void TestCameraScript::ToggleCamera()
{
    _playerCamera = !_playerCamera;
}