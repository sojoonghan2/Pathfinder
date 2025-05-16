#pragma once

class GameObject;
class Camera;
class Light;

// 모든 Scene의 부모 클래스
class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();
	virtual void Init();

	void Render();

	// 렌더링 파이프라인 단계별 분리
	void ClearRTV();
	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();
	void RenderForward();
	void PushLightData();

	// 게임 오브젝트 관리
	void AddGameObject(std::shared_ptr<GameObject> gameObject);
	void RemoveGameObject(std::shared_ptr<GameObject> gameObject);

	// 접근자
	std::shared_ptr<Camera> GetMainCamera();
	const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }

	void SetSelectedModuleType(int type) { _selectedModuleType = type; }
	int GetSelectedModuleType() const { return _selectedModuleType; }

	void SetName(const std::wstring& name) { _sceneName = name; }
	const std::wstring& GetName() const { return _sceneName; }

private:
	std::wstring _sceneName;
	std::vector<std::shared_ptr<GameObject>> _gameObjects;
	std::vector<std::shared_ptr<Camera>> _cameras;
	std::vector<std::shared_ptr<Light>> _lights;
	int _selectedModuleType = 0;
};