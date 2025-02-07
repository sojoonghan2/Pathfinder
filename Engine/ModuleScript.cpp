#include "pch.h"
#include "ModuleScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameModule.h"

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
    if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON)) // 마우스 왼쪽 버튼 클릭
    {
        // 현재 마우스 좌표 가져오기
        const POINT& pos = INPUT->GetMousePos();

        // 현재 씬에서 마우스 클릭한 오브젝트 찾기
        shared_ptr<GameObject> pickedObject = GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);

        // 피킹된 오브젝트가 존재하는지 확인
        if (pickedObject)
        {
            // GameModule 타입인지 확인 (다운캐스팅)
            shared_ptr<GameModule> pickedModule = dynamic_pointer_cast<GameModule>(pickedObject);

            if (pickedModule)
            {
                // 선택된 모듈 타입을 가져옴
                int selectedModuleType = pickedModule->GetModuleType();

                // 씬의 selectedModuleScript에 저장
                GET_SINGLE(SceneManager)->GetActiveScene()->SetSelectedModuleType(selectedModuleType);

                std::cout << "Selected Module Type: " << selectedModuleType + 1 << "\n-\n";
            }
        }

        // 기존 로직 유지
        if (alive && (way == WAIT))
        {
            way = DOWN;
        }
    }
}