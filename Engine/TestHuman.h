#pragma once
#pragma once
#include "MonoBehaviour.h"

enum class AnimationState 
{
	Idle,
	Walk
};


class TestHuman : public MonoBehaviour
{
public:
	float		_speed = 1000.f;
	float		anicount = 0;
	virtual void Update() override;
	float		animation_state = 0;

	float Get_state() {
		return animation_state;
	}
	void SetAnimationState(AnimationState state);
	AnimationState _currentState = AnimationState::Idle;

private:
};

