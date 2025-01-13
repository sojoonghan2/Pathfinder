#pragma once
#pragma once
#include "MonoBehaviour.h"

class TestHuman : public MonoBehaviour
{
public:
	float		_speed = 1000.f;
	float		anicount = 0;
	virtual void Update() override;
};

