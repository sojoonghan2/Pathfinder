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
    // ��� �����ֱ�
    if (INPUT->GetButton(KEY_TYPE::I) && (way == WAIT))
    {
        alive = true;
        way = UP;
    }

    // �̵�
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

    // �̵�
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