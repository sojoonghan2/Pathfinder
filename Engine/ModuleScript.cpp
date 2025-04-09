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
#include "RectangleCollider.h"

void ModuleScript::LateUpdate()
{
	KeyboardInput();
	MouseInput();
	if (selected) { SelectedAnimation(); }
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
		Vec3 pos = GetTransform()->GetWorldPosition();

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
		Vec3 pos = GetTransform()->GetWorldPosition();

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
		// ���� ���콺 ��ǥ ��������
		const POINT& pos = INPUT->GetMousePos();

		if (IsUIPicked(pos.x, pos.y))
		{
			shared_ptr<GameModule> gameModule = dynamic_pointer_cast<GameModule>(GetGameObject());
			GET_SINGLE(SceneManager)->GetActiveScene()->SetSelectedModuleType(gameModule->GetModuleType());
			selected = true;
		}

		// ���� ���� ����
		if (alive && (way == WAIT))
		{
			// �ӽ÷� �������� �ʵ���
			// way = DOWN;
		}
	}
}

bool ModuleScript::IsUIPicked(int mouseXpos, int mouseYpos)
{
	// ���콺 ����
	mouseXpos -= (WINDOWHEIGHT / 2);
	mouseYpos -= (WINDOWWIDTH / 2);

	int uiX = GetGameObject()->GetTransform()->GetWorldPosition().x;
	int uiY = GetGameObject()->GetTransform()->GetWorldPosition().y;

	int width = 150.f;
	int height = 500.f;

	// ���콺�� UI ���� �ȿ� �ִ��� Ȯ��
	return (mouseXpos >= uiX - width && mouseXpos <= uiX + width &&
		mouseYpos >= uiY - height && mouseYpos <= uiY + height);
}

void ModuleScript::SelectedAnimation()
{
	shared_ptr<GameModule> gameModule = dynamic_pointer_cast<GameModule>(GetGameObject());

	static float timeElapsed = 0.0f;
	timeElapsed += DELTA_TIME * 2.0f;

	float scaleX = 50.0f * cos(timeElapsed * 3.141592) + 350.0f;
	float scaleY = 50.0f * cos(timeElapsed * 3.141592) + 550.0f;

	gameModule->GetTransform()->SetLocalScale(Vec3(scaleX, scaleY, 0));

	if (timeElapsed >= 1.0f)
	{
		timeElapsed = 0.0f; // �ʱ�ȭ
		selected = false;   // ���� ����
	}
}