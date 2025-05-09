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
	//레이팅 티어 기본 VRS TIER 2
};
