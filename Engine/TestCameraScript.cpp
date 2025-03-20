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
    _offsetPosition = Vec3(0.f, 300.f, -300.f);
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
        // Ÿ���� �� �÷��̾� ������Ʈ�� ã��
        _target = GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ");

        // Ÿ���� ���� position�� �����ͼ� offset�� ����
        Vec3 targetPos = _target->GetTransform()->GetLocalPosition();
        GetTransform()->SetLocalPosition(targetPos + _offsetPosition);
    }
}

void TestCameraScript::KeyboardInput()
{
    Vec3 pos = GetTransform()->GetLocalPosition();

    if (!_playerCamera)
    {
        // �̵� ó��
        if (INPUT->GetButton(KEY_TYPE::W)) 
            pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::S))
            pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::A))
            pos -= Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::D))
            pos += Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;
    }
    // �÷��̾� ī�޶� ȸ�� ������ �ڵ�(���� ����)
    else
    {
        // �̵� ó��
        if (INPUT->GetButton(KEY_TYPE::W)) {
            _tempxRotation += 0.1;
            std::cout << "xRotation: " << _tempxRotation << "\n";
        }

        if (INPUT->GetButton(KEY_TYPE::S)) {
            _tempxRotation -= 0.1;
            std::cout << "xRotation: " << _tempxRotation << "\n";
        }

        if (INPUT->GetButton(KEY_TYPE::A)) {
            _tempyRotation += 0.1;
            std::cout << "yRotation: " << _tempyRotation << "\n";
        }

        if (INPUT->GetButton(KEY_TYPE::D)) {
            _tempyRotation -= 0.1;
            std::cout << "yRotation: " << _tempyRotation << "\n";
        }
    }

    // �� ũ�� ����
    float mapMinX = -4900.f;
    float mapMaxX = 4900.f;
    float mapMinZ = -4900.f;
    float mapMaxZ = 4900.f;
    float minY = 500.f;
    float maxY = 9500.f;

    // X, Y, Z ��ǥ�� �� ������ ����
    //pos.x = max(mapMinX, min(pos.x, mapMaxX));
    //pos.y = max(minY, min(pos.y, maxY));
    //pos.z = max(mapMinZ, min(pos.z, mapMaxZ));

    // ������ ��ǥ ���
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
    // �÷��̾� ī�޶� ����� ���� ���콺 �Է¿� ���� ȸ���� ó������ ����
    if (_playerCamera)
    {
        // �÷��̾� ī�޶� ��忡���� �ʱ� ȸ���� ����
        Vec3 initialRotation = Vec3(_tempxRotation, _tempyRotation, 0.f);
        GetTransform()->SetLocalRotation(initialRotation);
        return;
    }

    // ������ ī�޶� ��忡���� ���콺 ȸ�� ó��
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