#pragma once

template<typename T>
using HandleGetter = D3D12_CPU_DESCRIPTOR_HANDLE(*)(T);

// CBV/SRV/UAV 리소스를 GPU에 바인딩하기 위한 디스크립터 힙 관리 클래스
// 뷰 정보를 담은 구조체
// 각 디스크립터 레지스터에 해당하는 핸들을 설정하고, 명령 리스트에 커밋하는 기능을 제공함
class GraphicsDescriptorTable
{
public:
	void Init(uint32 count);
	void Clear();

	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);

	// 디스크립터 힙에 설정된 바인딩을 실제 GPU에 커밋
	void CommitTable();

	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }

	// 템플릿 핸들 접근자
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

// CBV/SRV/UAV 리소스를 GPU에 바인딩하기 위한 컴퓨트 디스크립터 힙 관리 클래스
// 뷰 정보를 담은 구조체
// 각 디스크립터 레지스터에 해당하는 핸들을 설정하고, 명령 리스트에 커밋하는 기능을 제공함
class ComputeDescriptorTable
{
public:
	void Init();

	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
	void SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UAV_REGISTER reg);

	// 디스크립터 힙에 설정된 바인딩을 실제 GPU에 커밋
	void CommitTable();

	template<typename T>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(T reg);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 reg);

private:
	ComPtr<ID3D12DescriptorHeap>	_descHeap;
	uint64							_handleSize{};
};