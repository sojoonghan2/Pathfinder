#include "pch.h"
#include "RenderTarget.h"
#include "GameFramework.h"
#include "Device.h"

void RenderTargetGroup::Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& renderTargets, shared_ptr<Texture> depthTexture)
{
	_groupType = groupType;
	_rtVec = renderTargets;
	_rtCount = static_cast<uint32>(renderTargets.size());
	_dsTexture = depthTexture;

	// RTV Èü »ý¼º
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = _rtCount;
	DEVICE->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));

	_rtvHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
	_dsvHeapBegin = _dsTexture->GetDSV()->GetCPUDescriptorHandleForHeapStart();

	CopyRTVs();
	PrepareResourceTransitions();
}

void RenderTargetGroup::CopyRTVs()
{
	for (uint32 i = 0; i < _rtCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE src = _rtVec[i].target->GetRTV()->GetCPUDescriptorHandleForHeapStart();
		D3D12_CPU_DESCRIPTOR_HANDLE dst = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, i * _rtvHeapSize);
		DEVICE->CopyDescriptorsSimple(1, dst, src, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
}

void RenderTargetGroup::PrepareResourceTransitions()
{
	assert(_rtCount <= 8 && "Too many MRTs");

	for (uint32 i = 0; i < _rtCount; ++i)
	{
		ID3D12Resource* resource = _rtVec[i].target->GetTex2D().Get();
		_targetToResource[i] = CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
		_resourceToTarget[i] = CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
}

void RenderTargetGroup::SetViewportAndScissorRect()
{
	const float w = static_cast<float>(_rtVec[0].target->GetWidth());
	const float h = static_cast<float>(_rtVec[0].target->GetHeight());

	D3D12_VIEWPORT viewport = { 0.f, 0.f, w, h, 0.f, 1.f };
	D3D12_RECT rect = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

	GRAPHICS_CMD_LIST->RSSetViewports(1, &viewport);
	GRAPHICS_CMD_LIST->RSSetScissorRects(1, &rect);
}

void RenderTargetGroup::OMSetRenderTargets(uint32 count, uint32 offset)
{
	SetViewportAndScissorRect();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, offset * _rtvHeapSize);
	GRAPHICS_CMD_LIST->OMSetRenderTargets(count, &rtvHandle, FALSE, &_dsvHeapBegin);
}

void RenderTargetGroup::OMSetRenderTargets()
{
	SetViewportAndScissorRect();
	GRAPHICS_CMD_LIST->OMSetRenderTargets(_rtCount, &_rtvHeapBegin, TRUE, &_dsvHeapBegin);
}

void RenderTargetGroup::ClearRenderTargetView(uint32 index)
{
	assert(index < _rtCount);

	auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, index * _rtvHeapSize);
	const float* clear = _rtVec[index].clearColor;

	GRAPHICS_CMD_LIST->ClearRenderTargetView(handle, clear, 0, nullptr);
	GRAPHICS_CMD_LIST->ClearDepthStencilView(_dsvHeapBegin, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderTargetGroup::ClearRenderTargetView()
{
	WaitResourceToTarget();

	for (uint32 i = 0; i < _rtCount; ++i)
	{
		ClearRenderTargetView(i);
	}
}

void RenderTargetGroup::WaitTargetToResource()
{
	GRAPHICS_CMD_LIST->ResourceBarrier(_rtCount, _targetToResource);
}

void RenderTargetGroup::WaitResourceToTarget()
{
	GRAPHICS_CMD_LIST->ResourceBarrier(_rtCount, _resourceToTarget);
}