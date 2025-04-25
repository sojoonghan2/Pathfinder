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
		Vec3 pos = GetTransform()->GetWorldPosition();

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
		Vec3 pos = GetTransform()->GetWorldPosition();

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
		// 현재 마우스 좌표 가져오기
		const POINT& pos = INPUT->GetMousePos();

		if (IsUIPicked(pos.x, pos.y))
		{
			shared_ptr<GameModule> gameModule = dynamic_pointer_cast<GameModule>(GetGameObject());
			GET_SINGLE(SceneManager)->GetActiveScene()->SetSelectedModuleType(gameModule->GetModuleType());
			selected = true;
		}

		// 기존 로직 유지
		if (alive && (way == WAIT))
		{
			// 임시로 내려가지 않도록
			// way = DOWN;
		}
	}
}

bool ModuleScript::IsUIPicked(int mouseXpos, int mouseYpos)
{
	// 마우스 보정
	mouseXpos -= (WINDOWHEIGHT / 2);
	mouseYpos -= (WINDOWWIDTH / 2);

	int uiX = GetGameObject()->GetTransform()->GetWorldPosition().x;
	int uiY = GetGameObject()->GetTransform()->GetWorldPosition().y;

	int width = 150.f;
	int height = 500.f;

	// 마우스가 UI 영역 안에 있는지 확인
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
		timeElapsed = 0.0f; // 초기화
		selected = false;   // 선택 해제
	}
}