#pragma once
#include "MasterScript.h"

class LoadingScript : public MasterScript
{
public:
    LoadingScript();
    virtual ~LoadingScript();

    virtual void LateUpdate() override;

    void StartLoadingThread();
    void SceneLoad();

private:
    std::thread*    loadThread;
    bool            loadEnd = false;
    bool            isInitialized = false;
};
