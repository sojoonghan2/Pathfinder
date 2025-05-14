#pragma once
#include "Object.h"
#include "Transform.h"
#include "BINLoader.h"

class Material;
class Buffer;

struct IndexBufferInfo
{
	ComPtr<ID3D12Resource>		buffer;
	D3D12_INDEX_BUFFER_VIEW		bufferView;
	DXGI_FORMAT					format;
	uint32						count;
};

struct KeyFrameInfo
{
	double	time;
	int32	frame;
	Vec3	scale;
	Vec4	rotation;
	Vec3	translate;
};

struct BoneInfo
{
	wstring					boneName;
	int32					parentIndex;
	Matrix					matOffset;
};

struct AnimClipInfo
{
	wstring			animName;
	int32			frameCount;
	double			duration;
	vector<vector<KeyFrameInfo>>	keyFrames;
};

class Mesh : public Object
{
public:
	Mesh();
	virtual ~Mesh();

	void Create(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexbuffer);
	void Render(uint32 instanceCount = 1, uint32 idx = 0);
	void Render(shared_ptr<class InstancingBuffer>& buffer, uint32 idx = 0);

	static shared_ptr<Mesh> CreateFromFBX(const struct FbxMeshInfo* meshInfo, class FBXLoader& loader);
	static shared_ptr<Mesh> CreateFromBIN(const struct BINInfo* meshInfo, shared_ptr<BINLoader> loader);
	////////////////////////////////////////////////////////////////////////// VRS �۾� �� //////////////////////////////////////////////////////////////////////////

	void CreateVRSImage(UINT width, UINT height);
	void UploadVRSData();
	ComPtr<ID3D12Resource> _vrsUploadBuffer;
	void CreateVRSUploadBuffer(UINT width, UINT height);

	////////////////////////////////////////////////////////////////////////// VRS �۾� �� //////////////////////////////////////////////////////////////////////////

	void SetTransform(shared_ptr<Transform> v) { _transform = v; };
	shared_ptr<Transform> GetTransform() { return _transform; };
	bool _supportsVRS = false;
	D3D12_VARIABLE_SHADING_RATE_TIER _shadingRateTier = D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED;

private:
	void CreateVertexBuffer(const vector<Vertex>& buffer);
	void CreateIndexBuffer(const vector<uint32>& buffer);
	void CreateBonesAndAnimations(class FBXLoader& loader);
	Matrix GetMatrix(FbxAMatrix& matrix);

public:
	uint32 GetSubsetCount() { return static_cast<uint32>(_vecIndexInfo.size()); }
	const vector<BoneInfo>* GetBones() { return &_bones; }
	uint32						GetBoneCount() { return static_cast<uint32>(_bones.size()); }
	const vector<AnimClipInfo>* GetAnimClip() { return &_animClips; }

	bool							IsAnimMesh() { return !_animClips.empty(); }
	shared_ptr<Buffer>	GetBoneFrameDataBuffer(int32 index = 0) { return _frameBuffer[index]; } // ��ü �� ������ ����
	shared_ptr<Buffer>	GetBoneOffsetBuffer() { return  _offsetBuffer; }

	void SetVrs(bool _vrs) { _supportsVRS = _vrs; }
	void SetRatingTier(D3D12_VARIABLE_SHADING_RATE_TIER rating_tier) { _shadingRateTier = rating_tier; }
	bool IsValid() const { return (_vertexBuffer != nullptr); }

private:
	ComPtr<ID3D12Resource>		_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	uint32 _vertexCount = 0;

	vector<IndexBufferInfo>		_vecIndexInfo;

	shared_ptr<Transform>       _transform;

	// Animation
	vector<AnimClipInfo>			_animClips;
	vector<BoneInfo>				_bones;
	D3D12_SHADING_RATE shading_rate = D3D12_SHADING_RATE_2X2;
	shared_ptr<Buffer>	_offsetBuffer; // �� ���� offset ���
	vector<shared_ptr<Buffer>> _frameBuffer; // ��ü �� ������ ����
};
