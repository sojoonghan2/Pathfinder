#pragma once
#include "MonoBehaviour.h"

enum class ModuleMoveState
{
	WAIT,
	UP,
	DOWN
};

class ModuleScript : public MonoBehaviour
{
public:
	void LateUpdate() override;

	void Activate();
	void Deactivate();
	void OnSelected();

private:
	void MoveUp();
	void MoveDown();
	void SelectedAnimation();

private:
	bool _alive = false;
	bool _selected = false;

	ModuleMoveState _moveState = ModuleMoveState::WAIT;

	float _speed = 3000.0f;
	const float _acceleration = 5000.0f;

	float _selectAnimTime = 0.0f;
};