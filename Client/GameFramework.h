#pragma once

#include "Device.h"
#include "CommandQueueAndList.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "Mesh.h"
#include "Shader.h"
#include "Buffer.h"
#include "ViewDescriptorHeap.h"
#include "Texture.h"
#include "RenderTarget.h"

// DirectX 12 기반의 렌더링 파이프라인 초기화 및 실행을 총괄하는 게임 프레임워크
// 디바이스, 커맨드 큐, 스왑체인, 루트 시그니처, 디스크립터 힙, 렌더 타겟 등의 생성 및 관리
// 프레임 렌더링 루프와 FPS 출력, 윈도우 리사이징 처리 등도 포함
// pch에서 유니크 포인터로 싱글톤처럼 생성하여 전역에서 참조가 가능하도록 함
class GameFramework
{
public:
	void Init(const WindowInfo& info);
	void Update();

public:
	const WindowInfo& GetWindow() { return _window; }
	shared_ptr<Device> GetDevice() { return _device; }
	shared_ptr<GraphicsCommandQueueAndList> GetGraphicsCmdQueue() { return _graphicsCmdQueue; }
	shared_ptr<ComputeCommandQueueAndList> GetComputeCmdQueue() { return _computeCmdQueue; }
	shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }
	shared_ptr<RootSignature> GetRootSignature() { return _rootSignature; }
	shared_ptr<GraphicsViewDescriptorHeap> GetGraphicsDescHeap() { return _graphicsDescHeap; }
	shared_ptr<ComputeViewDescriptorHeap> GetComputeDescHeap() { return _computeDescHeap; }

	shared_ptr<ConstantBuffer> GetConstantBuffer(CONSTANT_BUFFER_TYPE type) { return _constantBuffers[static_cast<uint8>(type)]; }
	shared_ptr<RenderTargetGroup> GetRTGroup(RENDER_TARGET_GROUP_TYPE type) { return _rtGroups[static_cast<uint8>(type)]; }

public:
	void Render();
	void RenderBegin();
	void RenderEnd();

	void ResizeWindow(int32 width, int32 height);
	void SetFullScreen();

private:
	void ShowFps();
	void CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count);
	void CreateRenderTargetGroups();

private:
	// 그려질 화면 크기 관련
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	shared_ptr<Device> _device = make_shared<Device>();
	shared_ptr<GraphicsCommandQueueAndList> _graphicsCmdQueue = make_shared<GraphicsCommandQueueAndList>();
	shared_ptr<ComputeCommandQueueAndList> _computeCmdQueue = make_shared<ComputeCommandQueueAndList>();
	shared_ptr<SwapChain> _swapChain = make_shared<SwapChain>();
	shared_ptr<RootSignature> _rootSignature = make_shared<RootSignature>();
	shared_ptr<GraphicsViewDescriptorHeap> _graphicsDescHeap = make_shared<GraphicsViewDescriptorHeap>();
	shared_ptr<ComputeViewDescriptorHeap> _computeDescHeap = make_shared<ComputeViewDescriptorHeap>();

	vector<shared_ptr<ConstantBuffer>> _constantBuffers;
	array<shared_ptr<RenderTargetGroup>, RENDER_TARGET_GROUP_COUNT> _rtGroups;
};
