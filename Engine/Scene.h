#pragma once

class GameObject;

class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();
	virtual void Init();

	shared_ptr<class Camera> GetMainCamera();

	void Render();

	void ClearRTV();
	D3D12_VARIABLE_SHADING_RATE_TIER Scene_RateTier = D3D12_VARIABLE_SHADING_RATE_TIER_2;
	void SetRenderingRate(D3D12_VARIABLE_SHADING_RATE_TIER Rate_tier);

	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();

	void RenderForward();

private:
	void PushLightData();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }
	const int GetSelectedModuleType() { return _selectedModuleType; }

	void SetSelectedModuleType(int type) { _selectedModuleType = type; }

	void SetName(const wstring& name) { _sceneName = name; }
	const wstring& GetName() const { return _sceneName; }

private:
	wstring								_sceneName;
	vector<shared_ptr<GameObject>>		_gameObjects;
	vector<shared_ptr<class Camera>>	_cameras;
	vector<shared_ptr<class Light>>		_lights;
	int									_selectedModuleType;
};
