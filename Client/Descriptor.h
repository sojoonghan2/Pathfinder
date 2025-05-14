#pragma once

template<typename T>
using HandleGetter = D3D12_CPU_DESCRIPTOR_HANDLE(*)(T);

class GraphicsDescriptorHeap
{
public:
	void Init(uint32 count);
	void Clear();

	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);

	// ��ũ���� ���� ������ ���ε��� ���� GPU�� Ŀ��
	void CommitTable();

	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }

	// ���ø� �ڵ� ������
	template<typename T>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(T reg);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 reg);

private:
	ComPtr<ID3D12DescriptorHeap>	_descHeap;
	uint64							_handleSize{};
	uint64							_groupSize{};
	uint64							_groupCount{};
	uint32							_currentGroupIndex{};
};


class ComputeDescriptorHeap
{
public:
	void Init();

	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
	void SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UAV_REGISTER reg);

	// ��ũ���� ���� ������ ���ε��� ���� GPU�� Ŀ��
	void CommitTable();

	template<typename T>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(T reg);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 reg);

private:
	ComPtr<ID3D12DescriptorHeap>	_descHeap;
	uint64							_handleSize{};
};