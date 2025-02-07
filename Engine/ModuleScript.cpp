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
    if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON))
    {
        // ��� �Ʒ��� ������
        if (alive && (way == WAIT))
        {
            way = DOWN;
            // ����� ��ŷ�ؼ� �ش� ����� type�� ������ �� ���� selectedModuleScript�� ����
            int selectedModuleType = ATKUp; // �ӽ�
            GET_SINGLE(SceneManager)->GetActiveScene()->SetSelectedModuleType(selectedModuleType);
            std::cout << "Selected Module Type: " << selectedModuleType << "\n-\n";
        }
    }
}
