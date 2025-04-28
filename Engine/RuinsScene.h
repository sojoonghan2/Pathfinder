#pragma once
#include "Scene.h"
class RuinsScene
{
public:
	RuinsScene();
	virtual ~RuinsScene();

	void Init();

	shared_ptr<Scene> GetScene() { return activeScene; }

private:
	shared_ptr<Scene> activeScene = make_shared<Scene>();

	bool stageClear;
};
