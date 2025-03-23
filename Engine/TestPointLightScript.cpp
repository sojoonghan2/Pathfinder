#include "pch.h"
#include "TestPointLightScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "MessageManager.h"
#include "SocketIO.h"
#include "Animator.h"

TestPointLightScript::TestPointLightScript()
{
}

TestPointLightScript::~TestPointLightScript()
{
}

void TestPointLightScript::LateUpdate()
{
#ifdef NETWORK_ENABLE

	// temp
	int id = GET_SINGLE(SocketIO)->myId;
	if (-1 != id) {
		auto& queue = GET_SINGLE(MessageManager)->GetMessageQueue(id);
		while (not queue.empty()) {
			auto& message = queue.front();
			SetPosition(message.first, message.second);
			queue.pop();
		}
	}

#endif // NETWORK_ENABLE
	_isMove = false;
	KeyboardInput();
	MouseInput();
	Animation();
}

void TestPointLightScript::KeyboardInput()
{
	// ���⼭ �޾Ƽ� ��ġ ��� �� 
	Vec3 pos = GetTransform()->GetLocalPosition();

	if (INPUT->GetButton(KEY_TYPE::W))
	{
		// ���� ���� ����� �ٸ� �����̸� �ʱ�ȭ(�� ȸ��)
		if (_way != PLAYERWAY_TYPE::FRONT)
		{
			GetTransform()->SetLocalRotation(Vec3(-1.5708f, 3.1416f, 0.0f));

			GetTransform()->SetLocalRotation(Vec3(-1.5708f, 3.1416f, 0.f));
		}

		_way = PLAYERWAY_TYPE::FRONT;

		pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
		_isMove = true;
	}
	if (INPUT->GetButton(KEY_TYPE::S))
	{
		// ���� ���� ����� �ٸ� �����̸� �ʱ�ȭ(�� ȸ��)
		if (_way != PLAYERWAY_TYPE::BACK)
		{
			GetTransform()->SetLocalRotation(Vec3(-1.5708f, 3.1416f, 0.0f));

			GetTransform()->SetLocalRotation(Vec3(-1.5708f, 0.0f, 0.f));
		}

		_way = PLAYERWAY_TYPE::BACK;

		pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
		_isMove = true;
	}
	if (INPUT->GetButton(KEY_TYPE::A))
	{
		// ���� ���� ����� �ٸ� �����̸� �ʱ�ȭ(�� ȸ��)
		if (_way != PLAYERWAY_TYPE::LEFT)
		{
			GetTransform()->SetLocalRotation(Vec3(-1.5708f, 3.1416f, 0.0f));

			GetTransform()->SetLocalRotation(Vec3(-1.5708f, 1.5708f, 0.f));
		}

		_way = PLAYERWAY_TYPE::LEFT;

		pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
		_isMove = true;
	}
	if (INPUT->GetButton(KEY_TYPE::D))
	{
		// ���� ���� ����� �ٸ� �����̸� �ʱ�ȭ(�� ȸ��)
		if (_way != PLAYERWAY_TYPE::RIGHT)
		{
			GetTransform()->SetLocalRotation(Vec3(-1.5708f, 3.1416f, 0.0f));

			GetTransform()->SetLocalRotation(Vec3(-1.5708f, -1.5708f, 0.f));
		}

		_way = PLAYERWAY_TYPE::RIGHT;

		pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
		_isMove = true;
	}

	if (INPUT->GetButtonDown(KEY_TYPE::T)) PRINTPOSITION;

	// �� ũ�� ����
	float mapMinX = -(MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f * METER_TO_CLIENT;
	float mapMaxX = (MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f * METER_TO_CLIENT;
	float mapMinZ = -(MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f * METER_TO_CLIENT;
	float mapMaxZ = (MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f * METER_TO_CLIENT;
	float minY = 0.f;
	float maxY = 9500.f;

	// X, Y, Z ��ǥ�� �� ������ ����
	pos.x = max(mapMinX, min(pos.x, mapMaxX));
	pos.y = max(minY, min(pos.y, maxY));
	pos.z = max(mapMinZ, min(pos.z, mapMaxZ));

	GetTransform()->SetLocalPosition(pos);
}

void TestPointLightScript::MouseInput()
{
	
}

void TestPointLightScript::Animation()
{
	static uint32 currentAnimIndex = 0;

	uint32 nextAnimIndex = _isMove ? 1 : 0;

	if (currentAnimIndex != nextAnimIndex)
	{
		GetAnimator()->Play(nextAnimIndex);
		currentAnimIndex = nextAnimIndex;
	}
}

void TestPointLightScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * METER_TO_CLIENT;
	pos.z = z * METER_TO_CLIENT;

	GetTransform()->SetLocalPosition(pos);
}