#pragma once
#include "Object.h"

enum class COMPONENT_TYPE : uint8
{
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	PARTICLE_SYSTEM,
	COLLIDER,
	ANIMATOR,
	// ...
	MONO_BEHAVIOUR,
	END,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END) - 1
};

class GameObject;
class Transform;
class MeshRenderer;
class Amature;

// GameObject에 부착 가능한 유니티의 컴포넌트의 기반 클래스
// 강의에서 배운 직관적인 구조로, 객체에 기능을 넣듯이 추가
class Component : public Object
{
public:
	Component(COMPONENT_TYPE type);
	virtual ~Component();

public:
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FinalUpdate() {}

public:
	COMPONENT_TYPE GetType() { return _type; }
	bool IsValid() { return _gameObject.expired() == false; }

	shared_ptr<GameObject> GetGameObject();
	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Amature> GetAmature();

private:
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

protected:
	COMPONENT_TYPE _type;
	weak_ptr<GameObject> _gameObject;
};
