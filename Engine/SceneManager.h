#pragma once

class Scene;

enum
{
	// 최대 레이어 수
	MAX_LAYER = 32
};

class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Init();
	void Update();
	void Render();
	void LoadScene(wstring sceneName);
	void ChangeScene(wstring sceneName);

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

	shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);

	shared_ptr<GameObject> FindObjectByName(const wstring& name);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }

private:

private:
	map<wstring, shared_ptr<Scene>> scenes;
	shared_ptr<Scene> _activeScene;

	array<wstring, MAX_LAYER> _layerNames;
	map<wstring, uint8> _layerIndex;
};

