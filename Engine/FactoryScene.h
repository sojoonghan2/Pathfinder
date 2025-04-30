#pragma once
#include "Scene.h"

class FactoryScene : public Scene
{
public:
	FactoryScene();
	virtual ~FactoryScene();

	void Init() override;

private:
	bool stageClear;
};
