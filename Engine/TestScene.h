#pragma once
#include "Scene.h"
#include "MonoBehaviour.h"

class TestScene : public MonoBehaviour{
public:
    TestScene();
    virtual ~TestScene();
    shared_ptr<Scene> GetScene() { return activeScene; }
private:
    shared_ptr<Scene> activeScene = make_shared<Scene>();
};
    