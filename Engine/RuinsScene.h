#pragma once
#include "Scene.h"
class RuinsScene {
public:
	RuinsScene();
	virtual ~RuinsScene();

	shared_ptr<Scene> GetScene() { return activeScene; }

private:
	shared_ptr<Scene> activeScene = make_shared<Scene>();

	bool stageClear;
};
