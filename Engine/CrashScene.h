#pragma once
#include "Scene.h"

class CrashScene : public Scene
{
public:
	CrashScene();
	virtual ~CrashScene();

	void Init() override;

private:
	bool stageClear;
};
