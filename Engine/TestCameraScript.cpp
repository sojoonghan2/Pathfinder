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
        // Ÿ���� �� �÷��̾� ������Ʈ�� ã��
        _target = GET_SINGLE(SceneManager)->FindObjectByName(L"OBJ");

        // 1. �÷��̾� ��ġ ��������
        Vec3 targetPos = _target->GetTransform()->GetLocalPosition();

        // 2. ī�޶� ������ ���� (���鿡�� ���� ���� X���� ũ��, Z���� �ణ�� ����)
        Vec3 offset = Vec3(500.f, 200.f, 100.f);  // X: ����Ÿ�, Y: ����, Z: �ణ�� ������ ���� ��

        // 3. ī�޶� ��ġ ����
        GetTransform()->SetLocalPosition(targetPos + offset);

        // 4. ���� ���� ��� �� ����ȭ
        Vec3 dirToTarget = targetPos - GetTransform()->GetLocalPosition();
        Vec3 dir = dirToTarget / dirToTarget.Length();

        // 5. ī�޶� ȸ�� ���� ��� 
        float pitch = asin(dir.y);
        float yaw = atan2(dir.x, dir.z);

        // 6. ī�޶� ȸ�� ����
        GetTransform()->SetLocalRotation(Vec3(pitch, yaw, 0.f));
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

    // �� ũ�� ����
    float mapMinX = -4900.f;
    float mapMaxX = 4900.f;
    float mapMinZ = -4900.f;
    float mapMaxZ = 4900.f;
    float minY = 500.f;
    float maxY = 9500.f;

    // X, Y, Z ��ǥ�� �� ������ ����
    pos.x = max(mapMinX, min(pos.x, mapMaxX));
    pos.y = max(minY, min(pos.y, maxY));
    pos.z = max(mapMinZ, min(pos.z, mapMaxZ));

    // ������ ��ǥ ���
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
	// ���콺 ������ ��ư�� ���� �ִ��� Ȯ��
	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		// ���콺 �̵����� ������ (�� ���� �����Ӹ��� ���ŵ�)
		POINT mouseDelta = INPUT->GetMouseDelta();

		// ���� ���� ȸ������ ������
		Vec3 rotation = GetTransform()->GetLocalRotation();

		// ���콺 X�� �̵��� Y�� ȸ���� ������ �� (�¿� ȸ��)
		rotation.y += mouseDelta.x * 0.005f;

		// ���콺 Y�� �̵��� X�� ȸ���� ������ �� (���� ȸ��)
		rotation.x += mouseDelta.y * 0.005f;

		// ȸ������ �ٽ� ����
		GetTransform()->SetLocalRotation(rotation);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		const POINT& pos2 = INPUT->GetMousePos();
		GET_SINGLE(SceneManager)->Pick(pos2.x, pos2.y);
	}
}

// TODO: �÷��̾� ī�޶�� ������ ī�޶� ��۷� �и�, ���� �ʿ�
// �θ� ������ �ϸ� ī�޶� Transform�� ������ �Ǵ� ���� ����
// ���̸� ���� �ٸ������� ����� �õ��ϸ� �������� ���� ���� ����
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