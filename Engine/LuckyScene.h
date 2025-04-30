#pragma once
#include "Scene.h"

class LuckyScene : public Scene
{
public:
	LuckyScene();
	virtual ~LuckyScene();

	void Init() override;

private:
	bool stageClear;
};
