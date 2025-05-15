#pragma once
#include "Component.h"

class Mesh;
class Material;

union InstanceID
{
	struct
	{
		uint32 meshID;
		uint32 materialID;
	};
	uint64 id;
};

// 메시와 머티리얼을 바탕으로 게임 오브젝트를 렌더링하는 컴포넌트
// 메시-머티리얼 조합을 기반으로 한 인스턴싱 식별자(InstanceID)를 계산함
// 간단하게 메시+머터리얼을 합한 구조
class MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	shared_ptr<Mesh> GetMesh() { return _mesh; }
	shared_ptr<Material> GetMaterial(uint32 idx = 0) { return _materials[idx]; }

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material, uint32 idx = 0);

	void Render();
	void Render(shared_ptr<class InstancingBuffer>& buffer);
	void RenderShadow();

	uint64 GetInstanceID();

private:
	shared_ptr<Mesh> _mesh;
	vector<shared_ptr<Material>> _materials;
};
