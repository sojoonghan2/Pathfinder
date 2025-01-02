#pragma once
#include "Scene.h"
class ParticleScene {
public:
    ParticleScene();
    virtual ~ParticleScene();

    shared_ptr<Scene> GetScene() { return activeScene; }
private:
    shared_ptr<Scene> activeScene = make_shared<Scene>();
};
