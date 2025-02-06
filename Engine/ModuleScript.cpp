#include "pch.h"
#include "ModuleScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

void ModuleScript::LateUpdate()
{
	KeyboardInput();
	MouseInput();
}

void ModuleScript::KeyboardInput()
{
    // 스테이지 클리어 후 모듈 활성화
    if (INPUT->GetButton(KEY_TYPE::I) && (way == WAIT)) // 임시용으로 I 누르면 모듈을 활성화
    {
        // 활성화 시키고 방향을 위로
        alive = true;
        way = UP;
    }

    // 위로 이동
    if (alive && (way == UP))
    {
        Vec3 pos = GetTransform()->GetLocalPosition();
        pos += Normalization(GetTransform()->GetUp()) * 1000.0f * DELTA_TIME;
        GetTransform()->SetLocalPosition(pos);

        if (GetTransform()->GetLocalPosition().y > 0.0f)
        {
            way = WAIT;
        }
    }

    // 아래로 이동
    if (alive && (way == DOWN))
    {
        Vec3 pos = GetTransform()->GetLocalPosition();
        pos -= Normalization(GetTransform()->GetUp()) * 1000.0f * DELTA_TIME;
        GetTransform()->SetLocalPosition(pos);

        if (GetTransform()->GetLocalPosition().y < -800.0f)
        {
            way = WAIT;
            alive = false;
        }
    }
}

void ModuleScript::MouseInput()
{
    if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON))
    {
        if (alive && (way == WAIT)) way = DOWN;
    }
}