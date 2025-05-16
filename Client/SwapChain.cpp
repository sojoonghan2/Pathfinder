// SwapChain.cpp - 리팩토링 버전
#include "pch.h"
#include "SwapChain.h"

namespace {
	constexpr DXGI_FORMAT BACKBUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr int REFRESH_RATE_NUMERATOR = 60;
	constexpr int REFRESH_RATE_DENOMINATOR = 1;
}

void SwapChain::Init(const WindowInfo& wndInfo, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory4> dxgiFactory, ComPtr<ID3D12CommandQueue> commandQueue)
{
	Create(wndInfo, dxgiFactory, commandQueue);
}

void SwapChain::Present()
{
	_swapChain->Present(0, 0);
}

void SwapChain::AdvanceBufferIndex()
{
	_backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}

void SwapChain::Create(const WindowInfo& wndInfo, ComPtr<IDXGIFactory4> dxgiFactory, ComPtr<ID3D12CommandQueue> commandQueue)
{
	_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
	swapDesc.BufferDesc.Width = static_cast<UINT>(wndInfo.width);
	swapDesc.BufferDesc.Height = static_cast<UINT>(wndInfo.height);
	swapDesc.BufferDesc.Format = BACKBUFFER_FORMAT;
	swapDesc.BufferDesc.RefreshRate.Numerator = REFRESH_RATE_NUMERATOR;
	swapDesc.BufferDesc.RefreshRate.Denominator = REFRESH_RATE_DENOMINATOR;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.OutputWindow = wndInfo.hwnd;
	swapDesc.Windowed = wndInfo.windowed;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	dxgiFactory->CreateSwapChain(commandQueue.Get(), &swapDesc, &_swapChain);
}