#pragma once

#include "Texture.h"

// ���� Ÿ�� �׷� Ÿ�� ����
enum class RENDER_TARGET_GROUP_TYPE : uint8
{
	SWAP_CHAIN,
	SHADOW,
	G_BUFFER,
	LIGHTING,
	REFLECTION,
	END
};

// ���� Ÿ�� ���� ���� ����
enum : uint8
{
	RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT = 1,
	RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT = 2,
	RENDER_TARGET_REFLECTION_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_GROUP_COUNT = static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::END)
};

// ���� Ÿ�� �ϳ��� ���� ���� (�ؽ�ó�� Ŭ���� ����)
struct RenderTarget
{
	shared_ptr<Texture> target;
	float clearColor[4] = { 0.f, 0.f, 0.f, 1.f };
};

// ���� Ÿ�� �׷�: ���� ���� Ÿ�ٰ� DSV�� ���� ����
class RenderTargetGroup
{
public:
	// �׷� �ʱ�ȭ
	void Create(RENDER_TARGET_GROUP_TYPE type, vector<RenderTarget>& rtList, shared_ptr<Texture> depthStencil);

	void CopyRTVs();
	void PrepareResourceTransitions();

	// ����Ʈ �� ���� ����
	void SetViewportAndScissorRect();

	// OM �ܰ� ���� Ÿ�� ���ε�
	void OMSetRenderTargets();
	void OMSetRenderTargets(uint32 count, uint32 offset);

	// ���� Ÿ�� Ŭ����
	void ClearRenderTargetView();
	void ClearRenderTargetView(uint32 index);

	// Getter
	shared_ptr<Texture> GetRTTexture(uint32 index) const { return _rtVec[index].target; }
	shared_ptr<Texture> GetDSTexture() const { return _dsTexture; }

	// ���ҽ� ���� ���� ����
	void WaitTargetToResource();
	void WaitResourceToTarget();

private:
	RENDER_TARGET_GROUP_TYPE		_groupType = RENDER_TARGET_GROUP_TYPE::END;

	vector<RenderTarget>			_rtVec;
	uint32							_rtCount{};
	shared_ptr<Texture>				_dsTexture;

	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	uint32							_rtvHeapSize{};
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHeapBegin{};
	D3D12_CPU_DESCRIPTOR_HANDLE		_dsvHeapBegin{};

	D3D12_RESOURCE_BARRIER			_targetToResource[8]{};
	D3D12_RESOURCE_BARRIER			_resourceToTarget[8]{};
};
