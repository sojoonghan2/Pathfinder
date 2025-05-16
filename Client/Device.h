#pragma once

// DirectX 12 ����̽� �� DXGI ���丮 ������ ����ϴ� Ŭ����
// ����̽� �ʱ�ȭ, ���丮 �� ����̽� ��ü�� �����ϴ� ��� ����
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
	// ȭ�� ���� ��ɵ�
	ComPtr<IDXGIFactory>		_dxgi;
	// ���� ��ü ����
	ComPtr<ID3D12Device>		_device;
};
