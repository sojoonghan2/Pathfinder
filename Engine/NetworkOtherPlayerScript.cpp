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
#include "Animator.h"

NetworkOtherPlayerScript::NetworkOtherPlayerScript()
{
	
}

NetworkOtherPlayerScript::~NetworkOtherPlayerScript()
{
}

void NetworkOtherPlayerScript::LateUpdate()
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
		default:
			break;
		}
		queue.pop();
	}


	Animation();

}

void NetworkOtherPlayerScript::Awake()
{
	GET_SINGLE(MessageManager)->RegisterObject(ObjectType::Player, _id);
}

void NetworkOtherPlayerScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * METER_TO_CLIENT;
	pos.y = -100.f;
	pos.z = z * METER_TO_CLIENT;

	GetTransform()->SetLocalPosition(pos);
}

void NetworkOtherPlayerScript::SetDir(float x, float z)
{
	Vec3 Look{ x, 0.f, z };
	Look.Normalize();
	if (Look.LengthSquared() > 0.0001f)
	{
		float targetYaw = atan2f(Look.x, Look.z) + XM_PI;
		Vec3 targetRot = Vec3(-XM_PIDIV2, targetYaw, 0.f);
		GetTransform()->SetLocalRotation(targetRot);
	}
}


void NetworkOtherPlayerScript::Animation()
{
	uint32 nextAnimIndex = 0;

	if (_isMove)
		nextAnimIndex = 1;
	if (!_isMove)
		nextAnimIndex = 0;
	if (_isShoot && _isMove)
		nextAnimIndex = 5;
	if (_isShoot && !_isMove)
		nextAnimIndex = 4;

	if (_currentAnimIndex != nextAnimIndex)
	{
		GetAnimator()->Play(nextAnimIndex);
		_currentAnimIndex = nextAnimIndex;
	}
}
