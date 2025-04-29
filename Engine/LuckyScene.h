#pragma once
#include "Scene.h"
class LuckyScene
{
public:
	LuckyScene();
	virtual ~LuckyScene();

	void Init();

	shared_ptr<Scene> GetScene() { return activeScene; }

private:
	shared_ptr<Scene> activeScene = make_shared<Scene>();

	bool stageClear;
};
