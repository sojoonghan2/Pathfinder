#pragma once
#include "Scene.h"

class ParticleScene : public Scene
{
public:
	ParticleScene();
	virtual ~ParticleScene();

	void Init() override;

	void LoadDebugParticle();
	void LoadMyParticle();

private:
	bool stageClear;
};