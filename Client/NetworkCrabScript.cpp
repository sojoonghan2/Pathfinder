#include "pch.h"
#include "NetworkCrabScript.h"
#include "Input.h"
#include "Timer.h"
#include "Amature.h"
#include "MeshData.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Amature.h"

#include "MessageManager.h"
#include "SocketIO.h"


NetworkCrabScript::NetworkCrabScript()
{}

void NetworkCrabScript::Start()
{
	_particlePool.Init(_particleObjects);
}

void NetworkCrabScript::LateUpdate()
{
	auto& queue = GET_SINGLE(MessageManager)->GetMessageQueue(_id);
	while (not queue.empty()) {
		auto& msg{ queue.front() };
		switch (msg->type) {
		case MsgType::MOVE:
		{
			std::shared_ptr<MsgMove> msg_move{
				std::static_pointer_cast<MsgMove>(msg) };
			SetPosition(msg_move->x, msg_move->y);
			// SetDir(message_move->dirX, message_move->dirY);

			// 움직임 감지
			if (_lastX != msg_move->x || _lastY != msg_move->y) {
				_isMove = true;
			}

			// 움직임 동일
			if (_lastX == msg_move->x && _lastY == msg_move->y) {
				_isMove = false;
			}
			_lastX = msg_move->x;
			_lastY = msg_move->y;

		}
		break;

		case MsgType::REGISTER:
		{
			GET_SINGLE(MessageManager)->RegisterObject(ObjectType::MONSTER, _id);
			SetPosition(100000.f, 0.f);
		}
		break;

		case MsgType::SET_OBJECT_HP:
		{
			auto msg_set_hp{ std::static_pointer_cast<MsgSetObjectHp>(msg) };
			_currentHp = msg_set_hp->hp;

			if (msg_set_hp->isAttacker) {
				msg_set_hp->maxHp = MONSTER_CRAB_HP;
				GET_SINGLE(MessageManager)->PushMessageByNetworkId(ID_OBJECT_MONSTER_HP, msg_set_hp);
			}

			// 파티클 출력
			auto particle = _particlePool.GetAvailable();
			if (particle)
			{
				particle->ParticleStart();
			}
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
	GET_SINGLE(MessageManager)->RegisterObject(ObjectType::MONSTER, _id);
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
		GetAmature()->Play(nextAnimIndex);
		_currentAnimIndex = nextAnimIndex;
	}
}