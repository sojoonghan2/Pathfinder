#pragma once

#include "Texture.h"

// 렌더 타겟 그룹 타입 구분
enum class RENDER_TARGET_GROUP_TYPE : uint8
{
	SWAP_CHAIN,
	SHADOW,
	G_BUFFER,
	LIGHTING,
	REFLECTION,
	END
};

// 렌더 타겟 구성 개수 정의
enum : uint8
{
	RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT = 1,
	RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT = 2,
	RENDER_TARGET_REFLECTION_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_GROUP_COUNT = static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::END)
};

// 렌더 타겟 하나의 구성 정보 (텍스처와 클리어 색상)
struct RenderTarget
{
	shared_ptr<Texture> target;
	float clearColor[4] = { 0.f, 0.f, 0.f, 1.f };
};

// 렌더 타겟 그룹: 여러 렌더 타겟과 DSV를 묶어 관리
class RenderTargetGroup
{
public:
	// 그룹 초기화
	void Create(RENDER_TARGET_GROUP_TYPE type, vector<RenderTarget>& rtList, shared_ptr<Texture> depthStencil);

	void CopyRTVs();
	void PrepareResourceTransitions();

	// 뷰포트 및 시저 설정
	void SetViewportAndScissorRect();

	// OM 단계 렌더 타겟 바인딩
	void OMSetRenderTargets();
	void OMSetRenderTargets(uint32 count, uint32 offset);

	// 렌더 타겟 클리어
	void ClearRenderTargetView();
	void ClearRenderTargetView(uint32 index);

	// Getter
	shared_ptr<Texture> GetRTTexture(uint32 index) const { return _rtVec[index].target; }
	shared_ptr<Texture> GetDSTexture() const { return _dsTexture; }

	// 리소스 상태 전이 제어
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
