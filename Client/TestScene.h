#pragma once
#include "Scene.h"

class TestScene : public Scene
{
public:
	TestScene();
	virtual ~TestScene();

	void Init() override;

private:
	bool stageClear;
};