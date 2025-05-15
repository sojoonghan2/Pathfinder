#pragma once

class SwapChain;
class DescriptorHeap;

// 커맨드 리스트를 GPU에 제출하기 위한 명령 큐 클래스
class GraphicsCommandQueueAndList
{
public:
	~GraphicsCommandQueueAndList();

	// 디바이스와 스왑체인을 공유 포인터를 통해 커멘드 큐 초기화
	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
	// GPU 작업 대기 펜스
	void WaitForGPUComplete();

	// 렌더링 시작
	void RenderBegin();
	// 렌더링 종료
	void RenderEnd();

	// 리소스 관련 커멘드 큐 작업 완료 대기
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

// 컴퓨트 커맨드 리스트를 GPU에 제출하기 위한 명령 큐 클래스
class ComputeCommandQueueAndList
{
public:
	~ComputeCommandQueueAndList();

	// 초기화
	void Init(ComPtr<ID3D12Device> device);
	// GPU 작업 대기 펜스
	void WaitForGPUComplete();
	// 컴퓨트 커멘드 큐 작업 완료 대기
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