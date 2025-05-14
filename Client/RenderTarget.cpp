#include "pch.h"
#include "RenderTarget.h"
#include "Application.h"
#include "Device.h"

// ���� Ÿ�� �׷� ���� �� �ʱ�ȭ
void RenderTargetGroup::Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture)
{
	_groupType = groupType;
	_rtVec = rtVec;
	_rtCount = static_cast<uint32>(rtVec.size());
	_dsTexture = dsTexture;

	// RTV �� ����
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = _rtCount;
	DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeap));

	_rtvHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
	_dsvHeapBegin = _dsTexture->GetDSV()->GetCPUDescriptorHandleForHeapStart();

	// RTV ����
	for (uint32 i = 0; i < _rtCount; i++)
	{
		auto srcHeap = _rtVec[i].target->GetRTV();
		auto srcHandle = srcHeap->GetCPUDescriptorHandleForHeapStart();
		auto destHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, i * _rtvHeapSize);

		DEVICE->CopyDescriptorsSimple(1, destHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// ���� ���� ����
	assert(_rtCount <= 8);
	for (uint32 i = 0; i < _rtCount; ++i)
	{
		ID3D12Resource* res = _rtVec[i].target->GetTex2D().Get();
		_targetToResource[i] = CD3DX12_RESOURCE_BARRIER::Transition(res, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
		_resourceToTarget[i] = CD3DX12_RESOURCE_BARRIER::Transition(res, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
}

void RenderTargetGroup::SetViewportAndScissor()
{
	const float w = static_cast<float>(_rtVec[0].target->GetWidth());
	const float h = static_cast<float>(_rtVec[0].target->GetHeight());

	D3D12_VIEWPORT vp = { 0.f, 0.f, w, h, 0.f, 1.f };
	D3D12_RECT rect = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

	GRAPHICS_CMD_LIST->RSSetViewports(1, &vp);
	GRAPHICS_CMD_LIST->RSSetScissorRects(1, &rect);
}

// �������� ���� ���� Ÿ�ٰ� ����/���ٽ� �並 ��� ���ձ�(OM)�� ����
void RenderTargetGroup::OMSetRenderTargets(uint32 count, uint32 offset)
{
	SetViewportAndScissor();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, offset * _rtvHeapSize);
	GRAPHICS_CMD_LIST->OMSetRenderTargets(count, &rtvHandle, FALSE, &_dsvHeapBegin);
}

// �������� ���� ���� Ÿ�ٰ� ����/���ٽ� �並 ��� ���ձ�(OM)�� ����
void RenderTargetGroup::OMSetRenderTargets()
{
	SetViewportAndScissor();

	GRAPHICS_CMD_LIST->OMSetRenderTargets(_rtCount, &_rtvHeapBegin, TRUE, &_dsvHeapBegin);
}

void RenderTargetGroup::ClearRenderTargetView(uint32 index)
{
	assert(index < _rtCount);

	auto rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, index * _rtvHeapSize);
	GRAPHICS_CMD_LIST->ClearRenderTargetView(rtvHandle, _rtVec[index].clearColor, 0, nullptr);
	GRAPHICS_CMD_LIST->ClearDepthStencilView(_dsvHeapBegin, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void RenderTargetGroup::ClearRenderTargetView()
{
	WaitResourceToTarget();

	for (uint32 i = 0; i < _rtCount; ++i)
	{
		auto rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, i * _rtvHeapSize);
		GRAPHICS_CMD_LIST->ClearRenderTargetView(rtvHandle, _rtVec[i].clearColor, 0, nullptr);
	}

	GRAPHICS_CMD_LIST->ClearDepthStencilView(_dsvHeapBegin, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

// ���� Ÿ�� -> ���ҽ� ���� ��ȯ
void RenderTargetGroup::WaitTargetToResource()
{
	GRAPHICS_CMD_LIST->ResourceBarrier(_rtCount, _targetToResource);
}

// ���ҽ� -> ���� Ÿ�� ���� ��ȯ
void RenderTargetGroup::WaitResourceToTarget()
{
	GRAPHICS_CMD_LIST->ResourceBarrier(_rtCount, _resourceToTarget);
}