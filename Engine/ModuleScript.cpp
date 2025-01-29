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
    // 이동 처리
    if (INPUT->GetButton(KEY_TYPE::I))
    {
        select = true;
    }

    if (select)
    {
        Vec3 pos = GetTransform()->GetLocalPosition();
        pos += Normalization(GetTransform()->GetUp()) * 1000.0f * DELTA_TIME;
        GetTransform()->SetLocalPosition(pos);
    }
}

void ModuleScript::MouseInput()
{
    
}