#include "pch.h"
#include "RenderTargetGroup.h"
#include "Engine.h"
#include "Device.h"

// ���� Ÿ�� �׷� ���� �� �ʱ�ȭ
void RenderTargetGroup::Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture)
{
	// ���� Ÿ�� �׷� �Ӽ� �ʱ�ȭ
	_groupType = groupType;
	_rtVec = rtVec;
	_rtCount = static_cast<uint32>(rtVec.size());
	_dsTexture = dsTexture;

	// ���� Ÿ�� �� �� ����
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = _rtCount;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;
	DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeap));

	// ���� Ÿ�� �� / ���� ���ٽ� �� �ڵ� ����
	_rtvHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
	_dsvHeapBegin = _dsTexture->GetDSV()->GetCPUDescriptorHandleForHeapStart();

	// ���� Ÿ�� �� ��ũ���͸� ���� ����
	for (uint32 i = 0; i < _rtCount; i++)
	{
		uint32 destSize = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE destHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, i * _rtvHeapSize);

		uint32 srcSize = 1;
		ComPtr<ID3D12DescriptorHeap> srcRtvHeapBegin = _rtVec[i].target->GetRTV();
		D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = srcRtvHeapBegin->GetCPUDescriptorHandleForHeapStart();

		DEVICE->CopyDescriptors(1, &destHandle, &destSize, 1, &srcHandle, &srcSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// ���� ��ȯ ���� �ʱ�ȭ
	for (int i = 0; i < _rtCount; ++i)
	{
		_targetToResource[i] = CD3DX12_RESOURCE_BARRIER::Transition(_rtVec[i].target->GetTex2D().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);

		_resourceToTarget[i] = CD3DX12_RESOURCE_BARRIER::Transition(_rtVec[i].target->GetTex2D().Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	// ��ũ�� �ؽ�ó�� ù ��° ���� Ÿ�� ����
	if (_rtCount > 0)
	{
		_screenTexture = rtVec[0].target;
	}

	CreateSRVForScreenTexture();
}

// ���� Ÿ�ٰ� ����/���ٽ� �並 ��� ���ձ�(OM)�� ����
void RenderTargetGroup::OMSetRenderTargets(uint32 count, uint32 offset)
{
	D3D12_VIEWPORT vp = D3D12_VIEWPORT{ 0.f, 0.f, _rtVec[0].target->GetWidth() , _rtVec[0].target->GetHeight(), 0.f, 1.f };
	D3D12_RECT rect = D3D12_RECT{ 0, 0, static_cast<LONG>(_rtVec[0].target->GetWidth()),  static_cast<LONG>(_rtVec[0].target->GetHeight()) };

	GRAPHICS_CMD_LIST->RSSetViewports(1, &vp);
	GRAPHICS_CMD_LIST->RSSetScissorRects(1, &rect);

	// ������ ���� Ÿ�� ��� ���� ���ٽ� �ڵ� OM�� ���ε�
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, offset * _rtvHeapSize);
	GRAPHICS_CMD_LIST->OMSetRenderTargets(count, &rtvHandle, FALSE/*1��*/, &_dsvHeapBegin);
}

void RenderTargetGroup::OMSetRenderTargets()
{
	D3D12_VIEWPORT vp = D3D12_VIEWPORT{ 0.f, 0.f, _rtVec[0].target->GetWidth() , _rtVec[0].target->GetHeight(), 0.f, 1.f };
	D3D12_RECT rect = D3D12_RECT{ 0, 0, static_cast<LONG>(_rtVec[0].target->GetWidth()),  static_cast<LONG>(_rtVec[0].target->GetHeight()) };

	GRAPHICS_CMD_LIST->RSSetViewports(1, &vp);
	GRAPHICS_CMD_LIST->RSSetScissorRects(1, &rect);

	GRAPHICS_CMD_LIST->OMSetRenderTargets(_rtCount, &_rtvHeapBegin, TRUE/*����*/, &_dsvHeapBegin);
}

void RenderTargetGroup::ClearRenderTargetView(uint32 index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, index * _rtvHeapSize);
	GRAPHICS_CMD_LIST->ClearRenderTargetView(rtvHandle, _rtVec[index].clearColor, 0, nullptr);

	GRAPHICS_CMD_LIST->ClearDepthStencilView(_dsvHeapBegin, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void RenderTargetGroup::ClearRenderTargetView()
{
	WaitResourceToTarget();

	for (uint32 i = 0; i < _rtCount; i++)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, i * _rtvHeapSize);
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

// ù ��° ���� Ÿ���� ��ũ�� �ؽ��ķ� ����
void RenderTargetGroup::SetScreenTexture()
{
	if (_rtCount > 0)
	{
		_screenTexture = _rtVec[0].target;
	}
}

void RenderTargetGroup::CreateSRVForScreenTexture()
{
	// �� ������ ��ȿ�� �˻�
	if (!_screenTexture || !_screenTexture->GetTex2D())
	{
		return;
	}

	// ���� �ؽ�ó �Ӽ� ��������
	D3D12_RESOURCE_DESC texDesc = _screenTexture->GetTex2D()->GetDesc();

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NodeMask = 0;

	ComPtr<ID3D12DescriptorHeap> srvHeap;
	HRESULT hr = DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	if (FAILED(hr)) return;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;  // ���� �ؽ�ó ���� ���
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	DEVICE->CreateShaderResourceView(_screenTexture->GetTex2D().Get(), &srvDesc,
		srvHeap->GetCPUDescriptorHandleForHeapStart());

	_screenTextureSRVHeap = srvHeap;
}

void RenderTargetGroup::BindScreenTextureToShader()
{
	if (!_screenTextureSRVHeap)
		return;

	ID3D12DescriptorHeap* heaps[] = { _screenTextureSRVHeap.Get() };
	GRAPHICS_CMD_LIST->SetDescriptorHeaps(_countof(heaps), heaps);

	// t2 �������Ϳ� ���ε�
	GRAPHICS_CMD_LIST->SetGraphicsRootDescriptorTable(1, _screenTextureSRVHeap->GetGPUDescriptorHandleForHeapStart());
}