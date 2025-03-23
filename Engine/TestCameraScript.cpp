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
        if (INPUT->GetButton(KEY_TYPE::UP)) 
            pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::DOWN))
            pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::LEFT))
            pos -= Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::RIGHT))
            pos += Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;
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
    // �÷��̾� ī�޶� ���
    if (_playerCamera)
    {
        if (INPUT->GetButton(KEY_TYPE::LBUTTON))
        {
            POINT mouseDelta = INPUT->GetMouseDelta();

            // Ÿ���� �����ϴ��� Ȯ��
            if (_target != nullptr)
            {
                // ���� ī�޶��� ȸ�� ���� (Ȥ�� ���� ����)�� �����ϴ� ���� ����
                static float totalYawAngle = 0.0f;

                // ���콺 �̵��� ���� ���� ������Ʈ
                totalYawAngle += mouseDelta.x * 0.005f;

                // Ÿ���� ��ġ ��������
                Vec3 targetPos = _target->GetTransform()->GetLocalPosition();

                // �������� ����(�Ÿ�) ���
                float distance = _offsetPosition.Length();

                // ���ο� ������ ��� (Y���� �״�� ����)
                Vec3 newOffset;
                newOffset.x = sin(totalYawAngle) * distance;
                newOffset.z = -cos(totalYawAngle) * distance;
                newOffset.y = _offsetPosition.y; // Y �������� ����

                // ī�޶� ��ġ ����
                GetTransform()->SetLocalPosition(targetPos + newOffset);

                // Ÿ���� �ٶ󺸵��� ���� (�ʿ��� ���)
                Vec3 dirToTarget = targetPos - (targetPos + newOffset);
                GetTransform()->LookAt(dirToTarget);
            }
        }
        return;
    }

    // ������ ī�޶� ���
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