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

    // �̵� ó��
    if (INPUT->GetButton(KEY_TYPE::W))
        pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

    if (INPUT->GetButton(KEY_TYPE::S))
        pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

    if (INPUT->GetButton(KEY_TYPE::A))
        pos -= Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

    if (INPUT->GetButton(KEY_TYPE::D))
        pos += Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;

    // �� ũ�� ����
    float mapMinX = -MAXMOVESIZE + 100;
    float mapMaxX = MAXMOVESIZE - 100;
    float mapMinZ = -MAXMOVESIZE + 100;
    float mapMaxZ = MAXMOVESIZE - 100;
    float minY = -MAXMOVESIZE + 1000;
    float maxY = MAXMOVESIZE;

    // X, Y, Z ��ǥ�� �� ������ ����
    pos.x = max(mapMinX, min(pos.x, mapMaxX));
    pos.y = max(minY, min(pos.y, maxY));
    pos.z = max(mapMinZ, min(pos.z, mapMaxZ));

    // ������ ��ǥ ���
    if (INPUT->GetButtonDown(KEY_TYPE::T))
        PRINTPOSITION;

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