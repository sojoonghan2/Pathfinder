#include "pch.h"
#include "GameFramework.h"
#include "Material.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Light.h"
#include "Resources.h"
#include "InstancingManager.h"
#include "SocketIO.h"
#include "CollisionManager.h"

void GameFramework::Init(const WindowInfo& info)
{
	_window = info;

	// 그려질 화면 크기를 설정
	_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	// 버퍼 사이즈가 부족한 경우 256, 512, 1024... 단위로 확장
	_device->Init();
	_graphicsCmdQueue->Init(_device->GetDevice(), _swapChain);
	_computeCmdQueue->Init(_device->GetDevice());
	ComPtr<IDXGIFactory4> factory4;
	_device->GetDXGI().As(&factory4);
	_swapChain->Init(info, _device->GetDevice(), factory4, _graphicsCmdQueue->GetCmdQueue());
	_rootSignature->Init();
	_graphicsDescHeap->Init(512);
	_computeDescHeap->Init();

	// 셰이더에 파라미터 형식으로 전달
	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 512);
	CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 512);

	CreateRenderTargetGroups();

	ResizeWindow(info.width, info.height);

	GET_SINGLE(Input)->Init(info.hwnd);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Resources)->Init();
	GET_SINGLE(SceneManager)->Init();
}

void GameFramework::Update()
{
	GET_SINGLE(Input)->Update();
	GET_SINGLE(Timer)->Update();

#ifdef NETWORK_ENABLE
	GET_SINGLE(SocketIO)->Update();
#endif

	if (GET_SINGLE(Input)->GetButtonDown(KEY_TYPE::ESC))
	{
		::PostQuitMessage(0);
		return;
	}

	GET_SINGLE(SceneManager)->Update();
	GET_SINGLE(CollisionManager)->Update();
	GET_SINGLE(InstancingManager)->ClearBuffer();

	Render();
}

void GameFramework::Render()
{
	RenderBegin();

	GET_SINGLE(SceneManager)->Render();

	ShowFps();

	RenderEnd();
}

void GameFramework::RenderBegin()
{
	_graphicsCmdQueue->RenderBegin();
}

void GameFramework::RenderEnd()
{
	_graphicsCmdQueue->RenderEnd();
}

void GameFramework::ResizeWindow(int32 width, int32 height)
{
	_window.width = width;
	_window.height = height;

	RECT rect = { 0, 0, width, height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	::SetWindowPos(_window.hwnd, 0, 100, 10, width, height, 0);
}

void GameFramework::ShowFps()
{
	uint32 fps = GET_SINGLE(Timer)->GetFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"Pathfinder (FPS: %d)", fps);

	::SetWindowText(_window.hwnd, text);
}

void GameFramework::CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count)
{
	uint8 typeInt = static_cast<uint8>(reg);
	assert(_constantBuffers.size() == typeInt);

	shared_ptr<ConstantBuffer> buffer = make_shared<ConstantBuffer>();
	buffer->Init(reg, bufferSize, count);
	_constantBuffers.push_back(buffer);
}

void GameFramework::CreateRenderTargetGroups()
{
	// 1. 렌더 타겟 텍스쳐 생성
	auto CreateRenderTargetTexture = [&](const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height, D3D12_RESOURCE_FLAGS flags) -> shared_ptr<Texture>
		{
			return GET_SINGLE(Resources)->CreateTexture(name, format, width, height,
				CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, flags);
		};

	vector<shared_ptr<Texture>> shadowTargets =
	{
		CreateRenderTargetTexture(L"ShadowTarget", DXGI_FORMAT_R32_FLOAT, 4096, 4096, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	};
	vector<shared_ptr<Texture>> GTargets =
	{
		CreateRenderTargetTexture(L"PositionTarget", DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		CreateRenderTargetTexture(L"NormalTarget", DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		CreateRenderTargetTexture(L"DiffuseTarget", DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	};
	vector<shared_ptr<Texture>> lightTargets =
	{
		CreateRenderTargetTexture(L"DiffuseLightTarget", DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		CreateRenderTargetTexture(L"SpecularLightTarget", DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	};
	vector<shared_ptr<Texture>> reflectionTargets =
	{
		CreateRenderTargetTexture(L"ReflectionPositionTarget", DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		CreateRenderTargetTexture(L"ReflectionNormalTarget", DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		CreateRenderTargetTexture(L"ReflectionDiffuseTarget", DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	};

	// 2. Depth-Stencil 텍스쳐 생성
	shared_ptr<Texture> depth = CreateRenderTargetTexture(L"ShadowDepthStencil", DXGI_FORMAT_D32_FLOAT, 4096, 4096, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	shared_ptr<Texture> reflectionDepth = CreateRenderTargetTexture(L"ReflectionDepthStencil", DXGI_FORMAT_D32_FLOAT, 4096, 4096, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	shared_ptr<Texture> depthStencilTexture = CreateRenderTargetTexture(L"DepthStencil", DXGI_FORMAT_D32_FLOAT, _window.width, _window.height, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	// 3. 렌더 타겟 그룹을 생성
	auto CreateRenderTargetGroup = [&](RENDER_TARGET_GROUP_TYPE groupType, const vector<shared_ptr<Texture>>& targets, shared_ptr<Texture> depth) -> void
		{
			vector<RenderTarget> rtVec(targets.size());
			for (size_t i = 0; i < targets.size(); ++i)
				rtVec[i].target = targets[i];

			_rtGroups[static_cast<uint8>(groupType)] = make_shared<RenderTargetGroup>();
			_rtGroups[static_cast<uint8>(groupType)]->Create(groupType, rtVec, depth);
		};

	// 4. G_Buffer에 렌더 타겟 텍스쳐에 깊이와 함께 합성
	CreateRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW, shadowTargets, depth);
	CreateRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER, GTargets, depthStencilTexture);
	CreateRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING, lightTargets, depthStencilTexture);
	CreateRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::REFLECTION, reflectionTargets, reflectionDepth);

	// 5. 스왑체인에서 얻은 백버퍼 리소스를 렌더 타겟으로 등록하여
	//	  화면 출력용 렌더 타겟 그룹(SWAP_CHAIN) 구성
	vector<shared_ptr<Texture>> renderTargets(SWAP_CHAIN_BUFFER_COUNT);
	for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		wstring name = L"SwapChainTarget_" + to_wstring(i);
		ComPtr<ID3D12Resource> resource;
		_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
		renderTargets[i] = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource);
	}
	CreateRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, renderTargets, depthStencilTexture);
}

void GameFramework::SetFullScreen()
{
	// 현재 윈도우 스타일 가져오기
	LONG style = ::GetWindowLong(_window.hwnd, GWL_STYLE);
	style &= ~WS_OVERLAPPEDWINDOW;
	style |= WS_POPUP;
	::SetWindowLong(_window.hwnd, GWL_STYLE, style);

	// 전체 화면으로 전환
	MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
	if (::GetMonitorInfo(MonitorFromWindow(_window.hwnd, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
	{
		const RECT& rc = monitorInfo.rcMonitor;
		::SetWindowPos(_window.hwnd, HWND_TOP,
			rc.left, rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			SWP_NOZORDER | SWP_FRAMECHANGED);
	}
}