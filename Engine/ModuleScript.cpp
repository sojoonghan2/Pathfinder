#include "pch.h"
#include "ModuleScript.h"
#include "Transform.h"
#include "GameObject.h"
#include "GameModule.h"
#include "Timer.h"

void ModuleScript::LateUpdate()
{
	if (_alive)
	{
		if (_moveState == ModuleMoveState::UP)
			MoveUp();
		else if (_moveState == ModuleMoveState::DOWN)
			MoveDown();
	}

	if (_selected)
		SelectedAnimation();
}

void ModuleScript::Activate()
{
	_alive = true;
	_moveState = ModuleMoveState::UP;
	_speed = 3000.0f;
}

void ModuleScript::Deactivate()
{
	_alive = true;
	_moveState = ModuleMoveState::DOWN;
	_speed = 3000.0f;
}

void ModuleScript::OnSelected()
{
	_selected = true;
	_selectAnimTime = 0.0f;
}

void ModuleScript::MoveUp()
{
	Vec3 pos = GetTransform()->GetWorldPosition();
	_speed -= _acceleration * DELTA_TIME;
	pos += Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
	GetTransform()->SetLocalPosition(pos);

	if (pos.y > 0.0f)
	{
		_moveState = ModuleMoveState::WAIT;
		_speed = 3000.0f;
	}
}

void ModuleScript::MoveDown()
{
	Vec3 pos = GetTransform()->GetWorldPosition();
	_speed -= _acceleration * DELTA_TIME;
	pos -= Normalization(GetTransform()->GetUp()) * _speed * DELTA_TIME;
	GetTransform()->SetLocalPosition(pos);

	if (pos.y < -800.0f)
	{
		_moveState = ModuleMoveState::WAIT;
		_alive = false;
		_speed = 3000.0f;
	}
}

void ModuleScript::SelectedAnimation()
{
	shared_ptr<GameModule> gameModule = dynamic_pointer_cast<GameModule>(GetGameObject());
	if (gameModule == nullptr) return;

	_selectAnimTime += DELTA_TIME * 2.0f;

	float scaleX = 50.0f * cos(_selectAnimTime * 3.141592f) + 350.0f;
	float scaleY = 50.0f * cos(_selectAnimTime * 3.141592f) + 550.0f;

	gameModule->GetTransform()->SetLocalScale(Vec3(scaleX, scaleY, 0));

	if (_selectAnimTime >= 1.0f)
	{
		_selected = false;
		_selectAnimTime = 0.0f;
	}
}