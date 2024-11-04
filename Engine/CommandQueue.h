#pragma once

class SwapChain;
class DescriptorHeap;

// ************************
// GraphicsCommandQueue
// ************************

// 그래픽스 명령 처리를 위한 커멘드 큐
class GraphicsCommandQueue
{
public:
	~GraphicsCommandQueue();

	// 디바이스와 스왑체인을 공유 포인터를 통해 커멘드 큐 초기화
	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
	// GPU 작업 대기 펜스
	void WaitSync();

	// 렌더링 시작
	void RenderBegin();
	// 렌더링 종료
	void RenderEnd();

	// 리소스 관련 커멘드 큐 작업 완료 대기
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

// 컴퓨트 쉐이더나 GPU 컴퓨트 작업을 처리하는 커멘드 큐
class ComputeCommandQueue
{
public:
	~ComputeCommandQueue();

	// 초기화
	void Init(ComPtr<ID3D12Device> device);
	// GPU 작업 대기 펜스
	void WaitSync();
	// 컴퓨트 커멘드 큐 작업 완료 대기
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