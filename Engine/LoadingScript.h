#include "MonoBehaviour.h"
#pragma once

enum class LoadingState
{
	Idle,
	ReadyToInit,
	ReadyToLoad,
	Switching
};

class Scene;

class LoadingScript : public MonoBehaviour
{
public:
	LoadingScript(RoomType type);
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
	LoadingState _state = LoadingState::Idle;

	shared_ptr<Scene> _sceneToInit;
	std::wstring _sceneNameToLoad;
};