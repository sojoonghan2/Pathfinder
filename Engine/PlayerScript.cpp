#include "pch.h"
#include "PlayerScript.h"
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

PlayerScript::PlayerScript()
{
}

PlayerScript::~PlayerScript()
{
}

void PlayerScript::LateUpdate()
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

void PlayerScript::KeyboardInput()
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

        // 목표 회전 계산
        float targetYaw = atan2f(moveDir.x, moveDir.z) + XM_PI;
        Vec3 currentRot = GetTransform()->GetLocalRotation();
        Vec3 targetRot = Vec3(-XM_PIDIV2, targetYaw, 0.f);

        // 정규화
        float deltaYaw = targetRot.y - currentRot.y;

        // 보정
        if (deltaYaw > XM_PI)
            deltaYaw -= XM_2PI;
        else if (deltaYaw < -XM_PI)
            deltaYaw += XM_2PI;

        // 보간 적용
        float lerpSpeed = 10.0f;
        currentRot.y += deltaYaw * lerpSpeed * DELTA_TIME;

        // 누적 회전값 정규화
        currentRot.y = fmod(currentRot.y, XM_2PI);
        if (currentRot.y < 0.f)
            currentRot.y += XM_2PI;

        GetTransform()->SetLocalRotation(currentRot);
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

void PlayerScript::MouseInput()
{
	
}

void PlayerScript::Animation()
{
	static uint32 currentAnimIndex = 0;

	uint32 nextAnimIndex = _isMove ? 1 : 0;

	if (currentAnimIndex != nextAnimIndex)
	{
		GetAnimator()->Play(nextAnimIndex);
		currentAnimIndex = nextAnimIndex;
	}
}

void PlayerScript::SetPosition(float x, float z)
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos.x = x * 200.f;
	pos.z = z * 200.f;

	GetTransform()->SetLocalPosition(pos);
}