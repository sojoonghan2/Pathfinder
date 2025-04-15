#include "pch.h"
#include "TestOtherPlayerScript.h"

#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "MessageManager.h"
#include "SocketIO.h"

TestOtherPlayerScript::TestOtherPlayerScript()
{
}

TestOtherPlayerScript::~TestOtherPlayerScript()
{
}

void TestOtherPlayerScript::LateUpdate()
{
#ifdef NETWORK_ENABLE

	if (-1 == id) {
		id = GET_SINGLE(SocketIO)->GetNextId();
	}

	if (-1 != id) {
		auto& queue = GET_SINGLE(MessageManager)->GetMessageQueue(id);
		while (not queue.empty()) {
			auto& message{ queue.front() };
			switch (message->type)
			{
			case MsgType::MOVE:
			{
				std::shared_ptr<MsgMove> message_move{
					std::static_pointer_cast<MsgMove>(message) };
				SetPosition(message_move->x, message_move->y);
				SetDir(message_move->dirX, message_move->dirY);
			}
			break;
			default:
				break;
			}
			queue.pop();
		}
	}

#endif // NETWORK_ENABLE
}

void TestOtherPlayerScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * METER_TO_CLIENT;
	pos.y = 500.f;
	pos.z = z * METER_TO_CLIENT;

	GetTransform()->SetLocalPosition(pos);
}

void TestOtherPlayerScript::SetDir(float x, float z)
{
	GetTransform()->LookAt(Vec3{ x, 0, z });
}
