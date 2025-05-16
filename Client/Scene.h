#pragma once

class GameObject;
class Camera;
class Light;

// ��� Scene�� �θ� Ŭ����
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

	// ������ ���������� �ܰ躰 �и�
	void ClearRTV();
	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();
	void RenderForward();
	void PushLightData();

	// ���� ������Ʈ ����
	void AddGameObject(std::shared_ptr<GameObject> gameObject);
	void RemoveGameObject(std::shared_ptr<GameObject> gameObject);

	// ������
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