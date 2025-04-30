#pragma once
#include "MasterScript.h"
#include "Scene.h"

class Scene;

class LoadingScript : public MonoBehaviour
{
public:
    LoadingScript();
    virtual ~LoadingScript();

    virtual void Awake() override;
    virtual void LateUpdate() override;

private:
    void StartLoadingThread();
    void SceneLoad();

private:
    bool loadEnd = false;
    bool isInitialized = false;
    bool pendingThreadJoin = false;

    std::thread* loadThread = nullptr;

    RoomType roomType = RoomType::None;

    shared_ptr<Scene> _sceneToInit;
    wstring _sceneNameToLoad;
};
