#pragma once
#include "Scene.h"

class LoadingScene : public Scene
{
public:
	LoadingScene();
	virtual ~LoadingScene();

	virtual void Init(RoomType type);

private:
	bool stageClear;
};