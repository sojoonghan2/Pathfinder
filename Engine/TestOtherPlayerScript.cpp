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
			auto& message = queue.front();
			SetPosition(message.first, message.second);
			queue.pop();
		}
	}

#endif // NETWORK_ENABLE

}


void TestOtherPlayerScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * 100.f;
	pos.z = z * 100.f;

	GetTransform()->SetLocalPosition(pos);
}
