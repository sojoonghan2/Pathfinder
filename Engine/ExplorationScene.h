#pragma once
#include "Scene.h"

class ExplorationScene : public Scene
{
public:
	ExplorationScene();
	virtual ~ExplorationScene();

	void Init() override;

private:
	bool stageClear;
};
