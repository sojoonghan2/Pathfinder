#include "pch.h"
#include "Device.h"

void Device::Init()
{
	CreateDXGIFactory2(0, IID_PPV_ARGS(&_dxgi));

	IDXGIAdapter1* pd3dAdapter = NULL;
	if(SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_device))));
}