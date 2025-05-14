#pragma once

// 스왑체인을 생성하고 백버퍼 관리, Present 처리
class SwapChain
{
public:
	void Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory4> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();
	void SwapIndex();

	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }
	uint8 GetBackBufferIndex() { return static_cast<uint8>(_backBufferIndex); }

private:
	void CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory4> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);

private:
	ComPtr<IDXGISwapChain>	_swapChain;
	uint32					_backBufferIndex = 0;
};