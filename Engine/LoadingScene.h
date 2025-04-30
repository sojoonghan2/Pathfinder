#pragma once
#include "Scene.h"

class LoadingScene : public Scene
{
public:
	LoadingScene();
	virtual ~LoadingScene();

	void Init() override;

private:
	bool stageClear;
};