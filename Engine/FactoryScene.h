#pragma once
#include "Scene.h"
class FactoryScene {
public:
    FactoryScene();
    virtual ~FactoryScene();

    shared_ptr<Scene> GetScene() { return activeScene; }
private:
    shared_ptr<Scene> activeScene = make_shared<Scene>();
};
