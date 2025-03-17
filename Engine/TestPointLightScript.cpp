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

	if (INPUT->GetButton(KEY_TYPE::UP))
	{
		pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
		_isMove = true;
	}
	if (INPUT->GetButton(KEY_TYPE::DOWN))
	{
		pos -= Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
		_isMove = true;
	}
	if (INPUT->GetButton(KEY_TYPE::LEFT))
	{
		pos += Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;
		_isMove = true;
	}
	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		pos -= Normalization(GetTransform()->GetRight()) * _speed * DELTA_TIME;
		_isMove = true;
	}

	if (INPUT->GetButton(KEY_TYPE::PAGEUP))
		pos += Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::PAGEDOWN))
		pos -= Normalization(GetTransform()->GetLook()) * _speed * DELTA_TIME;

	if (INPUT->GetButtonDown(KEY_TYPE::T)) PRINTPOSITION;

	// �� ũ�� ����
	float mapMinX = -4950.f;
	float mapMaxX = 4950.f;
	float mapMinZ = -4950.f;
	float mapMaxZ = 4950.f;
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
	pos.x = x * 200.f;
	pos.z = z * 200.f;

	GetTransform()->SetLocalPosition(pos);
}
