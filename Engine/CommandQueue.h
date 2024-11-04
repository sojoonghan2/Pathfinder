#pragma once

class SwapChain;
class DescriptorHeap;

// ************************
// GraphicsCommandQueue
// ************************

// �׷��Ƚ� ��� ó���� ���� Ŀ��� ť
class GraphicsCommandQueue
{
public:
	~GraphicsCommandQueue();

	// ����̽��� ����ü���� ���� �����͸� ���� Ŀ��� ť �ʱ�ȭ
	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
	// GPU �۾� ��� �潺
	void WaitSync();

	// ������ ����
	void RenderBegin();
	// ������ ����
	void RenderEnd();

	// ���ҽ� ���� Ŀ��� ť �۾� �Ϸ� ���
	void FlushResourceCommandQueue();

	// Getter
	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCmdList() { return	_cmdList; }
	ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return	_resCmdList; }

private:
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	ComPtr<ID3D12CommandAllocator>		_resCmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_resCmdList;

	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;

	shared_ptr<SwapChain>				_swapChain;
};

// ************************
// ComputeCommandQueue
// ************************

// ��ǻƮ ���̴��� GPU ��ǻƮ �۾��� ó���ϴ� Ŀ��� ť
class ComputeCommandQueue
{
public:
	~ComputeCommandQueue();

	// �ʱ�ȭ
	void Init(ComPtr<ID3D12Device> device);
	// GPU �۾� ��� �潺
	void WaitSync();
	// ��ǻƮ Ŀ��� ť �۾� �Ϸ� ���
	void FlushComputeCommandQueue();

	// Getter
	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetComputeCmdList() { return _cmdList; }

private:
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;
};