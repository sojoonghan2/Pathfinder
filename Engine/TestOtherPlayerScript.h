#pragma once
#include "MonoBehaviour.h"

class TestOtherPlayerScript : public MonoBehaviour
{
public:
	TestOtherPlayerScript();
	virtual ~TestOtherPlayerScript();

	virtual void LateUpdate() override;

	void SetPosition(float x, float z);

private:
	float		_speed = 1000.f;
	int			id = -1;
};
