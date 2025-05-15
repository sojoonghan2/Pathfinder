#include "pch.h"
#include "CommandQueueAndList.h"
#include "SwapChain.h"
#include "GameFramework.h"

GraphicsCommandQueueAndList::~GraphicsCommandQueueAndList()
{
	::CloseHandle(_fenceEvent);
}

void GraphicsCommandQueueAndList::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
	HRESULT hResult;

	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC cmdQDesc{};
	cmdQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hResult = device->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(&_cmdQueue));
	hResult = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	hResult = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
	hResult = _cmdList->Close();

	hResult = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAllocator));
	hResult = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));

	hResult = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void GraphicsCommandQueueAndList::WaitForGPUComplete()
{
	_fenceValue++;

	HRESULT hResult = _cmdQueue->Signal(_fence.Get(), _fenceValue);

	if (_fence->GetCompletedValue() < _fenceValue)
	{
		hResult = _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void GraphicsCommandQueueAndList::RenderBegin()
{
	HRESULT hResult;

	hResult = _cmdAllocator->Reset();
	hResult = _cmdList->Reset(_cmdAllocator.Get(), nullptr);

	int8 backIndex = _swapChain->GetBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	_cmdList->SetGraphicsRootSignature(GRAPHICS_ROOT_SIGNATURE.Get());

	GFramework->GetConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM)->Clear();
	GFramework->GetConstantBuffer(CONSTANT_BUFFER_TYPE::MATERIAL)->Clear();

	GFramework->GetGraphicsDescHeap()->Clear();

	ID3D12DescriptorHeap* descHeap = GFramework->GetGraphicsDescHeap()->GetDescriptorHeap().Get();
	_cmdList->SetDescriptorHeaps(1, &descHeap);

	_cmdList->ResourceBarrier(1, &barrier);
}

void GraphicsCommandQueueAndList::RenderEnd()
{
	HRESULT hResult;

	int8 backIndex = _swapChain->GetBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GFramework->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);

	_cmdList->ResourceBarrier(1, &barrier);
	hResult = _cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	_swapChain->Present();

	WaitForGPUComplete();

	_swapChain->SwapIndex();
}

void GraphicsCommandQueueAndList::UploadResource()
{
	HRESULT hResult;

	hResult = _resCmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	WaitForGPUComplete();

	hResult = _resCmdAllocator->Reset();
	hResult = _resCmdList->Reset(_resCmdAllocator.Get(), nullptr);
}

ComputeCommandQueueAndList::~ComputeCommandQueueAndList()
{
	::CloseHandle(_fenceEvent);
}

void ComputeCommandQueueAndList::Init(ComPtr<ID3D12Device> device)
{
	HRESULT hResult;

	D3D12_COMMAND_QUEUE_DESC computecmdQDesc{};
	computecmdQDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	computecmdQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hResult = device->CreateCommandQueue(&computecmdQDesc, IID_PPV_ARGS(&_cmdQueue));
	hResult = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&_cmdAllocator));
	hResult = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
	hResult = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void ComputeCommandQueueAndList::WaitForGPUComplete()
{
	_fenceValue++;

	HRESULT hResult = _cmdQueue->Signal(_fence.Get(), _fenceValue);

	if (_fence->GetCompletedValue() < _fenceValue)
	{
		hResult = _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void ComputeCommandQueueAndList::UploadComputeResource()
{
	HRESULT hResult;

	hResult = _cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	WaitForGPUComplete();

	hResult = _cmdAllocator->Reset();
	hResult = _cmdList->Reset(_cmdAllocator.Get(), nullptr);

	COMPUTE_CMD_LIST->SetComputeRootSignature(COMPUTE_ROOT_SIGNATURE.Get());
}