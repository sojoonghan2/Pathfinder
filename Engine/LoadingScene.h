#pragma once
#include "Scene.h"

class LoadingScene {
public:
    LoadingScene();
    virtual ~LoadingScene();
    shared_ptr<Scene> GetScene() { return activeScene; }

private:
    shared_ptr<Scene> activeScene = make_shared<Scene>();
    
    bool stageClear;
};