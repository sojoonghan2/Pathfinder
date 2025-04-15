#include "pch.h"
#include "NetworkOtherPlayerScript.h"

#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "MessageManager.h"
#include "SocketIO.h"

NetworkOtherPlayerScript::NetworkOtherPlayerScript()
{
}

NetworkOtherPlayerScript::~NetworkOtherPlayerScript()
{
}

void NetworkOtherPlayerScript::LateUpdate()
{

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

}

void NetworkOtherPlayerScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * METER_TO_CLIENT;
	pos.y = 0.f;
	pos.z = z * METER_TO_CLIENT;

	GetTransform()->SetLocalPosition(pos);
}

void NetworkOtherPlayerScript::SetDir(float x, float z)
{
	Vec3 Look{ x, 0.f, z };
	if (Look.LengthSquared() > 0.0001f)
	{
		float targetYaw = atan2f(Look.x, Look.z) + XM_PI;
		Vec3 targetRot = Vec3(-XM_PIDIV2, targetYaw, 0.f);
		GetTransform()->SetLocalRotation(targetRot);
	}
}
