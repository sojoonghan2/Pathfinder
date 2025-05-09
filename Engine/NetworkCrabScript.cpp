#include "pch.h"
#include "NetworkCrabScript.h"
#include "Input.h"
#include "Timer.h"
#include "Animator.h"
#include "MeshData.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator.h"

#include "MessageManager.h"
#include "SocketIO.h"


NetworkCrabScript::NetworkCrabScript()
{}

void NetworkCrabScript::LateUpdate()
{
	auto& queue = GET_SINGLE(MessageManager)->GetMessageQueue(_id);
	while (not queue.empty()) {
		auto& message{ queue.front() };
		switch (message->type) {
		case MsgType::MOVE:
		{
			std::shared_ptr<MsgMove> message_move{
				std::static_pointer_cast<MsgMove>(message) };
			SetPosition(message_move->x, message_move->y);
			// SetDir(message_move->dirX, message_move->dirY);

			// 움직임 감지
			if (_lastX != message_move->x || _lastY != message_move->y) {
				_isMove = true;
			}

			// 움직임 동일
			if (_lastX == message_move->x && _lastY == message_move->y) {
				_isMove = false;
			}
			_lastX = message_move->x;
			_lastY = message_move->y;

		}
		break;

		case MsgType::REGISTER:
		{
			GET_SINGLE(MessageManager)->RegisterObject(ObjectType::Monster, _id);
			SetPosition(100000.f, 0.f);
		}
		break;
		default:
			break;
		}
		queue.pop();
	}

	Animation();
}

void NetworkCrabScript::Awake()
{
	GET_SINGLE(MessageManager)->RegisterObject(ObjectType::Monster, _id);
}

void NetworkCrabScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * METER_TO_CLIENT;
	pos.y = 0.f;
	pos.z = z * METER_TO_CLIENT;

	GetTransform()->SetLocalPosition(pos);
}

void NetworkCrabScript::SetDir(float x, float z)
{
	Vec3 Look{ x, 0.f, z };	
	GetTransform()->SetLocalRotation(Look);
}

void NetworkCrabScript::Animation()
{
	uint32 nextAnimIndex = 0;

	if (_isMove)
		nextAnimIndex = 1;
	else
		nextAnimIndex = 0;

	if (_currentAnimIndex != nextAnimIndex)
	{
		GetAnimator()->Play(nextAnimIndex);
		_currentAnimIndex = nextAnimIndex;
	}
}