#pragma once

enum class OBJECT_TYPE : uint8
{
	NONE,
	GAMEOBJECT,
	COMPONENT,
	MATERIAL,
	MESH,
	MESH_DATA,
	SHADER,
	TEXTURE,

	END
};

enum
{
	OBJECT_TYPE_COUNT = static_cast<uint8>(OBJECT_TYPE::END)
};

// 리소스 및 게임 오브젝트의 공통 속성을 정의하는 기반 클래스
class Object
{
public:
	Object(OBJECT_TYPE type);
	virtual ~Object();

	OBJECT_TYPE GetType() { return _objectType; }

	void SetName(const wstring& name) { _name = name; }
	const wstring& GetName() { return _name; }

	uint32 GetID() { return _id; }

protected:
	friend class Resources;
	virtual void Load(const wstring& path) {}
	virtual void Save(const wstring& path) {}
	virtual void SaveOrLoad(const wstring& filePath) {}

protected:
	OBJECT_TYPE _objectType = OBJECT_TYPE::NONE;
	wstring _name;

protected:
	uint32 _id = 0;
};
