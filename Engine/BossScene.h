#pragma once
#include "Scene.h"
class BossScene {
public:
	BossScene();
	virtual ~BossScene();

	shared_ptr<Scene> GetScene() { return activeScene; }

private:
	shared_ptr<Scene> activeScene = make_shared<Scene>();

	bool stageClear;
};
