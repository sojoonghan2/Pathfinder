#pragma once

class Device
{
public:
	void Init();

	ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
	ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
	ComPtr<ID3D12Debug>			_debugController;
	// 화면 관련 기능들
	ComPtr<IDXGIFactory>		_dxgi;
	// 각종 객체 생성
	ComPtr<ID3D12Device>		_device;
};

