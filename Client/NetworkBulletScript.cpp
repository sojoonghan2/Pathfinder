#include "pch.h"
#include "NetworkBulletScript.h"

#include "MessageManager.h"
#include "Transform.h"

NetworkBulletScript::NetworkBulletScript()
{
}

NetworkBulletScript::~NetworkBulletScript()
{
}

void NetworkBulletScript::LateUpdate()
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
			SetDir(message_move->dirX, message_move->dirY);
		}
		break;
		case MsgType::REGISTER:
		{
			GET_SINGLE(MessageManager)->RegisterObject(ObjectType::BULLET, _id);
			SetPosition(100000.f, 0.f);
		}
		break;
		default:
			break;
		}
		queue.pop();
	}
}


void NetworkBulletScript::Awake()
{
	GET_SINGLE(MessageManager)->RegisterObject(ObjectType::BULLET, _id);
}


void NetworkBulletScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * METER_TO_CLIENT;
	pos.y = 400.f;
	pos.z = z * METER_TO_CLIENT;

	GetTransform()->SetLocalPosition(pos);
}

void NetworkBulletScript::SetDir(float x, float z)
{
	Vec3 Look{ x, 0.f, z };
	GetTransform()->SetLocalRotation(Look);
}
