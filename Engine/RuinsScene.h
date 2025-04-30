#pragma once
#include "Scene.h"

class RuinsScene : public Scene
{
public:
	RuinsScene();
	virtual ~RuinsScene();

	void Init() override;

private:
	bool stageClear = false;
};
