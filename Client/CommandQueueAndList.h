#pragma once

class SwapChain;
class DescriptorHeap;

// Ŀ�ǵ� ����Ʈ�� GPU�� �����ϱ� ���� ��� ť Ŭ����
class GraphicsCommandQueueAndList
{
public:
	~GraphicsCommandQueueAndList();

	// ����̽��� ����ü���� ���� �����͸� ���� Ŀ��� ť �ʱ�ȭ
	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
	// GPU �۾� ��� �潺
	void WaitForGPUComplete();

	// ������ ����
	void RenderBegin();
	// ������ ����
	void RenderEnd();

	// ���ҽ� ���� Ŀ��� ť �۾� �Ϸ� ���
	void UploadResource();

	// Getter
	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCmdList() { return	_cmdList; }
	ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return	_resCmdList; }

private:
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	ComPtr<ID3D12CommandAllocator>		_resCmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	_resCmdList;

	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;

	shared_ptr<SwapChain>				_swapChain;
};

// ��ǻƮ Ŀ�ǵ� ����Ʈ�� GPU�� �����ϱ� ���� ��� ť Ŭ����
class ComputeCommandQueueAndList
{
public:
	~ComputeCommandQueueAndList();

	// �ʱ�ȭ
	void Init(ComPtr<ID3D12Device> device);
	// GPU �۾� ��� �潺
	void WaitForGPUComplete();
	// ��ǻƮ Ŀ��� ť �۾� �Ϸ� ���
	void UploadComputeResource();

	// Getter
	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetComputeCmdList() { return _cmdList; }

private:
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;
};