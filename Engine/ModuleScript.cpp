#include "pch.h"
#include "ModuleScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"

void ModuleScript::LateUpdate()
{
    KeyboardInput();
    MouseInput();
}

void ModuleScript::KeyboardInput()
{
    // 스테이지 클리어 후 모듈 활성화
    if (INPUT->GetButton(KEY_TYPE::I) && (way == WAIT))
    {
        alive = true;
        way = UP;
        _speed = 3000.0f; // 이동 시작 시 속도를 최대 속도로 설정
    }

    // 위로 이동
    if (alive && (way == UP))
    {
        Vec3 pos = GetTransform()->GetLocalPosition();

        _speed -= _acceleration * DELTA_TIME;

        pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
        GetTransform()->SetLocalPosition(pos);

        if (pos.y > 0.0f)
        {
            way = WAIT;
            _speed = 3000.0f; // 이동 종료 후 속도 초기화
        }
    }

    // 아래로 이동
    if (alive && (way == DOWN))
    {
        Vec3 pos = GetTransform()->GetLocalPosition();

        _speed -= _acceleration * DELTA_TIME;

        pos -= Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
        GetTransform()->SetLocalPosition(pos);

        if (pos.y < -800.0f)
        {
            way = WAIT;
            alive = false;
            _speed = 3000.0f; // 이동 종료 후 속도 초기화
        }
    }
}

void ModuleScript::MouseInput()
{
    if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON))
    {
        // 모듈 아래로 내리기
        if (alive && (way == WAIT))
        {
            way = DOWN;
            // 모듈을 피킹해서 해당 모듈의 type을 가져온 후 씬의 selectedModuleScript에 저장
            int selectedModuleType = ATKUp; // 임시
            GET_SINGLE(SceneManager)->GetActiveScene()->SetSelectedModuleType(selectedModuleType);
            std::cout << "Selected Module Type: " << selectedModuleType << "\n-\n";
        }
    }
}
