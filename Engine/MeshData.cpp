#include "pch.h"
#include "MeshData.h"
#include "FBXLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Animator.h"

MeshData::MeshData() : Object(OBJECT_TYPE::MESH_DATA)
{
}

MeshData::~MeshData()
{
}

shared_ptr<MeshData> MeshData::LoadFromFBX(const wstring& path)
{
	FBXLoader loader;
	loader.LoadFbx(path);

	shared_ptr<MeshData> meshData = make_shared<MeshData>();

	for (int32 i = 0; i < loader.GetMeshCount(); i++)
	{
		shared_ptr<Mesh> mesh = Mesh::CreateFromFBX(&loader.GetMesh(i), loader);

		if (mesh->GetName().empty())
		{
			// 이름이 없을 경우 기본 이름 설정
			wstring defaultName = L"Mesh_" + to_wstring(i);
			mesh->SetName(defaultName);
		}

		GET_SINGLE(Resources)->Add<Mesh>(mesh->GetName(), mesh);

		// Material 찾아서 연동
		vector<shared_ptr<Material>> materials;
		for (size_t j = 0; j < loader.GetMesh(i).materials.size(); j++)
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(loader.GetMesh(i).materials[j].name);
			materials.push_back(material);
		}

		MeshRenderInfo info = {};
		info.mesh = mesh;
		info.materials = materials;
		meshData->_meshRenders.push_back(info);
	}

	return meshData;
}

void MeshData::Load(const wstring& filePath)
{
	ifstream file(filePath, ios::binary);
	if (!file.is_open())
	{
		wcout << L"Failed to open file for loading: " << filePath << endl;
		return;
	}

	// 디버깅 로그
	wcout << L"Starting to load mesh data from: " << filePath << endl;

	// 메쉬 렌더 정보 로드
	size_t meshRenderCount = 0;
	file.read(reinterpret_cast<char*>(&meshRenderCount), sizeof(size_t));

	if (meshRenderCount == 0)
	{
		wcout << L"No mesh render data found in file." << endl;
		file.close();
		return;
	}

	for (size_t i = 0; i < meshRenderCount; ++i)
	{
		MeshRenderInfo info = {};

		// 메쉬 이름 로드
		size_t meshNameSize = 0;
		file.read(reinterpret_cast<char*>(&meshNameSize), sizeof(size_t));
		if (meshNameSize > 0)
		{
			wstring meshName(meshNameSize, L'\0');
			file.read(reinterpret_cast<char*>(meshName.data()), meshNameSize * sizeof(wchar_t));

			info.mesh = GET_SINGLE(Resources)->Get<Mesh>(meshName);
		}
		else
		{
			wcout << L"Warning: Mesh name is empty in file." << endl;
		}

		// 머티리얼 로드
		size_t materialCount = 0;
		file.read(reinterpret_cast<char*>(&materialCount), sizeof(size_t));

		for (size_t j = 0; j < materialCount; ++j)
		{
			size_t materialNameSize = 0;
			file.read(reinterpret_cast<char*>(&materialNameSize), sizeof(size_t));
			if (materialNameSize > 0)
			{
				wstring materialName(materialNameSize, L'\0');
				file.read(reinterpret_cast<char*>(materialName.data()), materialNameSize * sizeof(wchar_t));

				shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(materialName);
				info.materials.push_back(material);
			}
			else
			{
				wcout << L"Warning: Material name is empty in file." << endl;
			}
		}

		_meshRenders.push_back(info);
	}

	file.close();
	wcout << L"Mesh data successfully loaded from: " << filePath << endl;
}

void MeshData::Save(const wstring& filePath)
{
	ofstream file(filePath, ios::binary);
	if (!file.is_open())
	{
		wcout << L"Failed to open file for saving: " << filePath << endl;
		return;
	}

	// 디버깅 로그
	wcout << L"Starting to save mesh data to: " << filePath << endl;

	// 메쉬 렌더 정보 저장
	size_t meshRenderCount = _meshRenders.size();
	file.write(reinterpret_cast<const char*>(&meshRenderCount), sizeof(size_t));

	if (meshRenderCount == 0)
	{
		wcout << L"No mesh render data to save." << endl;
		file.close();
		return;
	}

	for (const auto& info : _meshRenders)
	{
		wstring meshName = info.mesh ? info.mesh->GetName() : L"";
		if (meshName.empty())
		{
			wcout << L"Warning: Mesh name is empty. Assigning default name." << endl;
			meshName = L"UnnamedMesh";
		}
		size_t meshNameSize = meshName.size();
		file.write(reinterpret_cast<const char*>(&meshNameSize), sizeof(size_t));
		file.write(reinterpret_cast<const char*>(meshName.data()), meshNameSize * sizeof(wchar_t));
		
		// 머티리얼 저장
		size_t materialCount = info.materials.size();
		file.write(reinterpret_cast<const char*>(&materialCount), sizeof(size_t));

		for (const auto& material : info.materials)
		{
			wstring materialName = material ? material->GetName() : L"";
			size_t materialNameSize = materialName.size();
			file.write(reinterpret_cast<const char*>(&materialNameSize), sizeof(size_t));

			if (materialNameSize > 0)
			{
				file.write(reinterpret_cast<const char*>(materialName.data()), materialNameSize * sizeof(wchar_t));
			}
			else
			{
				wcout << L"Warning: Material name is empty for one of the materials." << endl;
			}
		}
	}

	file.flush();
	file.close();

	wcout << L"Mesh data successfully saved to: " << filePath << endl;
}

void MeshData::SaveOrLoad(const wstring& savePath, const wstring& fbxPath)
{
	namespace fs = std::filesystem;

	if (fs::exists(savePath))
	{
		// 저장된 MeshData가 있으면 로드
		wcout << L"Loading mesh data from: " << savePath << endl;
		Load(savePath);
	}
	else
	{
		// 저장된 MeshData가 없으면 FBX 로드 후 저장
		wcout << L"FBX file detected, saving to: " << savePath << endl;
		shared_ptr<MeshData> tempMeshData = GET_SINGLE(Resources)->LoadFBX(fbxPath);

		// FBX 데이터를 현재 MeshData로 복사
		_meshRenders = tempMeshData->_meshRenders;

		// 저장
		wcout << L"Saving FBX: " << savePath << endl;
		Save(savePath);
	}
}

vector<shared_ptr<GameObject>> MeshData::Instantiate()
{
	vector<shared_ptr<GameObject>> v;

	for (MeshRenderInfo& info : _meshRenders)
	{
		shared_ptr<GameObject> gameObject = make_shared<GameObject>();
		gameObject->AddComponent(make_shared<Transform>());
		gameObject->AddComponent(make_shared<MeshRenderer>());
		gameObject->GetMeshRenderer()->SetMesh(info.mesh);

		for (uint32 i = 0; i < info.materials.size(); i++)
			gameObject->GetMeshRenderer()->SetMaterial(info.materials[i], i);

		if (info.mesh->IsAnimMesh())
		{
			shared_ptr<Animator> animator = make_shared<Animator>();
			gameObject->AddComponent(animator);
			animator->SetBones(info.mesh->GetBones());
			animator->SetAnimClip(info.mesh->GetAnimClip());
		}

		v.push_back(gameObject);
	}


	return v;
}

