#pragma once
#include "Scene.h"

class BossScene : public Scene
{
public:
	BossScene();
	virtual ~BossScene();

	void Init() override;

private:
	bool stageClear;
};
