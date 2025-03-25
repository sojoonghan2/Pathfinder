#include "pch.h"
#include "TestPointLightScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
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
    Vec3 pos = GetTransform()->GetLocalPosition();

    // 카메라 방향 기준 벡터 획득
    shared_ptr<GameObject> cameraObj = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetGameObject();
    Vec3 camForward = cameraObj->GetTransform()->GetLook();
    Vec3 camRight = cameraObj->GetTransform()->GetRight();
    camForward.y = 0;
    camRight.y = 0;
    camForward.Normalize();
    camRight.Normalize();

    Vec3 moveDir = Vec3(0.f);

    // 입력 기반 이동 방향 누적
    if (INPUT->GetButton(KEY_TYPE::W)) moveDir += camForward;
    if (INPUT->GetButton(KEY_TYPE::S)) moveDir -= camForward;
    if (INPUT->GetButton(KEY_TYPE::A)) moveDir -= camRight;
    if (INPUT->GetButton(KEY_TYPE::D)) moveDir += camRight;

    if (moveDir.LengthSquared() > 0.01f)
    {
        moveDir.Normalize();
        pos += moveDir * _speed * DELTA_TIME;
        _isMove = true;

        // 이동 방향을 바라보도록 회전 (언리얼 스타일)
        float yaw = atan2f(moveDir.x, moveDir.z);
        yaw += XM_PI;
        GetTransform()->SetLocalRotation(Vec3(-XM_PIDIV2, yaw, 0.f));
    }
    else
    {
        _isMove = false;
    }

    // 맵 크기 제한
    float mapMinX = -4950.f;
    float mapMaxX = 4950.f;
    float mapMinZ = -4950.f;
    float mapMaxZ = 4950.f;
    float minY = 0.f;
    float maxY = 9500.f;

    // X, Y, Z 좌표를 맵 범위로 제한
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