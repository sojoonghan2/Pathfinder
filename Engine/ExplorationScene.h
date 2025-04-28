#pragma once
#include "Scene.h"
class ExplorationScene
{
public:
	ExplorationScene();
	virtual ~ExplorationScene();

	void Init();

	shared_ptr<Scene> GetScene() { return activeScene; }

private:
	shared_ptr<Scene> activeScene = make_shared<Scene>();

	bool stageClear;
};
