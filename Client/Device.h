#pragma once

// DirectX 12 디바이스 및 DXGI 팩토리 생성을 담당하는 클래스
// 디바이스 초기화, 팩토리 및 디바이스 객체에 접근하는 기능 제공
class Device
{
public:
	void Init();
	void CreateFactory();
	void CreateHardwareDevice();

	ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
	ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
	ComPtr<ID3D12Debug>			_debugController;
	// 화면 관련 기능들
	ComPtr<IDXGIFactory>		_dxgi;
	// 각종 객체 생성
	ComPtr<ID3D12Device>		_device;
};
