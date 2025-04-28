#pragma once
#include "Scene.h"
class CrashScene
{
public:
	CrashScene();
	virtual ~CrashScene();

	void Init();

	shared_ptr<Scene> GetScene() { return activeScene; }

private:
	shared_ptr<Scene> activeScene = make_shared<Scene>();

	bool stageClear;
};
