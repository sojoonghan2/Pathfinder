#pragma once
#include "Scene.h"
class TitleScene {
public:
    TitleScene();
    virtual ~TitleScene();

    shared_ptr<Scene> GetScene() { return activeScene; }

private:
    shared_ptr<Scene> activeScene = make_shared<Scene>();

    bool stageClear;
};
