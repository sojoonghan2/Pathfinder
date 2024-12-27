#pragma once
#include "Object.h"

class Mesh;
class Material;
class GameObject;

struct MeshRenderInfo
{
	shared_ptr<Mesh>				mesh;
	vector<shared_ptr<Material>>	materials;
};

class MeshData : public Object
{
public:
	MeshData();
	virtual ~MeshData();

public:
	static shared_ptr<MeshData> LoadFromFBX(const wstring& path);
	static shared_ptr<MeshData> LoadFromBIN(const wstring& path);

	virtual void Load(const wstring& path);
	virtual void Save(const wstring& path);
	virtual void SaveOrLoad(const wstring& savePath, const wstring& fbxPath);

	vector<shared_ptr<GameObject>> Instantiate();

private:
	shared_ptr<Mesh>				_mesh;
	vector<shared_ptr<Material>>	_materials;

	vector<MeshRenderInfo> _meshRenders;
};
