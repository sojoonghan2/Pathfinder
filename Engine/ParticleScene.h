#pragma once
#include "Scene.h"
class ParticleScene {
public:
	ParticleScene();
	virtual ~ParticleScene();

	void LoadMyParticle();
	void LoadDebugParticle();

	void Init();

	shared_ptr<Scene> GetScene() { return activeScene; }
private:
	shared_ptr<Scene> activeScene = make_shared<Scene>();

	bool stageClear;
};
