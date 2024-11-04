#pragma once

enum class CONSTANT_BUFFER_TYPE : uint8
{
	GLOBAL,
	TRANSFORM,
	MATERIAL,
	END
};

enum
{
	CONSTANT_BUFFER_COUNT = static_cast<uint8>(CONSTANT_BUFFER_TYPE::END)
};

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	// 초기화
	void Init(CBV_REGISTER reg, uint32 size, uint32 count);

	// 상수 버퍼를 비움(인덱스를 초기화하거나 메모리 재설정)
	void Clear();
	// 그래픽스 파이프라인에서 사용할 데이터의 내용을 상수 버퍼에 복사
	void PushGraphicsData(void* buffer, uint32 size);
	// 그래픽스 파이프라인의 전역 데이터를 상수 버퍼에 복사
	void SetGraphicsGlobalData(void* buffer, uint32 size);
	// 컴퓨트 파이프라인에서 사용할 데이터 내용 상수 버퍼에 복사
	void PushComputeData(void* buffer, uint32 size);

	// 지정된 인덱스에 해당하는 상수 버퍼의 GPU 가상 주소 반환
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);
	// 상수 버퍼에 대한 CPU 쪽 핸들 반환
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);

private:
	// 상수 버퍼 리소스 생성
	void CreateBuffer();
	// 상수 버퍼 뷰 생성
	void CreateView();

private:
	ComPtr<ID3D12Resource>				_cbvBuffer;
	BYTE*								_mappedBuffer = nullptr;
	uint32								_elementSize = 0;
	uint32								_elementCount = 0;

	ComPtr<ID3D12DescriptorHeap>		_cbvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE			_cpuHandleBegin = {};
	uint32								_handleIncrementSize = 0;

	uint32								_currentIndex = 0;

	CBV_REGISTER						_reg = {};
};

