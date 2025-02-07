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
    // �������� Ŭ���� �� ��� Ȱ��ȭ
    if (INPUT->GetButton(KEY_TYPE::I) && (way == WAIT))
    {
        alive = true;
        way = UP;
        _speed = 3000.0f; // �̵� ���� �� �ӵ��� �ִ� �ӵ��� ����
    }

    // ���� �̵�
    if (alive && (way == UP))
    {
        Vec3 pos = GetTransform()->GetLocalPosition();

        _speed -= _acceleration * DELTA_TIME;

        pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
        GetTransform()->SetLocalPosition(pos);

        if (pos.y > 0.0f)
        {
            way = WAIT;
            _speed = 3000.0f; // �̵� ���� �� �ӵ� �ʱ�ȭ
        }
    }

    // �Ʒ��� �̵�
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
            _speed = 3000.0f; // �̵� ���� �� �ӵ� �ʱ�ȭ
        }
    }
}

void ModuleScript::MouseInput()
{
    if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON)) // ���콺 ���� ��ư Ŭ��
    {
        // ���� ���콺 ��ǥ ��������
        const POINT& pos = INPUT->GetMousePos();

        // ���� ������ ���콺 Ŭ���� ������Ʈ ã��
        shared_ptr<GameObject> pickedObject = GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);

        // ��ŷ�� ������Ʈ�� �����ϴ��� Ȯ��
        if (pickedObject)
        {
            // GameModule Ÿ������ Ȯ�� (�ٿ�ĳ����)
            shared_ptr<GameModule> pickedModule = dynamic_pointer_cast<GameModule>(pickedObject);

            if (pickedModule)
            {
                // ���õ� ��� Ÿ���� ������
                int selectedModuleType = pickedModule->GetModuleType();

                // ���� selectedModuleScript�� ����
                GET_SINGLE(SceneManager)->GetActiveScene()->SetSelectedModuleType(selectedModuleType);

                std::cout << "Selected Module Type: " << selectedModuleType + 1 << "\n-\n";
            }
        }

        // ���� ���� ����
        if (alive && (way == WAIT))
        {
            way = DOWN;
        }
    }
}