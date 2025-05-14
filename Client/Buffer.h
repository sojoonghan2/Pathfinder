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

class Buffer
{
public:
	Buffer();
	~Buffer();

	void Init(uint32 elementSize, uint32 elementCount, void* initialData = nullptr);

	// 그래픽스 파이프라인에서 해당 버퍼를 SRV로 바인딩
	void PushGraphicsData(SRV_REGISTER reg);
	// 컴퓨팅 파이프라인에서 해당 버퍼를 SRV로 바인딩
	void PushComputeSRVData(SRV_REGISTER reg);
	// 컴퓨팅 파이프라인에서 해당 버퍼를 UAV로 바인딩
	void PushComputeUAVData(UAV_REGISTER reg);

	ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAV() { return _uavHeap; }

	// 버퍼의 현재 상태
	void SetResourceState(D3D12_RESOURCE_STATES state) { _resourceState = state; }
	D3D12_RESOURCE_STATES GetResourceState() { return _resourceState; }
	ComPtr<ID3D12Resource> GetBuffer() { return _buffer; }

	uint32	GetElementSize() { return _elementSize; }
	uint32	GetElementCount() { return _elementCount; }
	UINT	GetBufferSize() { return _elementSize * _elementCount; }

	void Update(const void* data, size_t dataSize);
	void CopyInitialData(uint64 bufferSize, void* initialData);

private:
	ComPtr<ID3D12Resource>			_buffer;
	ComPtr<ID3D12DescriptorHeap>	_srvHeap;
	ComPtr<ID3D12DescriptorHeap>	_uavHeap;

	uint32						_elementSize{};
	uint32						_elementCount{};
	D3D12_RESOURCE_STATES		_resourceState{};

	D3D12_CPU_DESCRIPTOR_HANDLE _srvHeapBegin{};
	D3D12_CPU_DESCRIPTOR_HANDLE _uavHeapBegin{};
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
	BYTE*								_mappedBuffer{ nullptr };
	uint32								_elementSize{};
	uint32								_elementCount{};

	ComPtr<ID3D12DescriptorHeap>		_cbvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE			_cpuHandleBegin{};
	uint32								_handleIncrementSize{};

	uint32								_currentIndex{};

	CBV_REGISTER						cbvResiger{};
};

struct InstancingParams
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
};

class InstancingBuffer
{
public:
	InstancingBuffer();
	~InstancingBuffer();

	// 인스턴스 개수 초기화
	void Init(uint32 maxCount = 10);

	// 저장된 인스턴싱 데이터 초기화, 이전 프레임 데이터 제거
	void Clear();
	// 인스턴싱 데이터 추가, 인스턴싱에 필요한 행렬을 포함하는 구조체를 받아 벡터에 추가
	void AddData(InstancingParams& params);
	// 벡터에 저장된 인스터싱 데이터 GPU 버퍼로 전송
	void PushData();

	// GetterSetter
	uint32						GetCount() { return static_cast<uint32>(_data.size()); }
	ComPtr<ID3D12Resource>		GetBuffer() { return _buffer; }
	D3D12_VERTEX_BUFFER_VIEW	GetBufferView() { return _bufferView; }

	void						SetID(uint64 instanceId) { _instanceId = instanceId; }
	uint64						GetID() { return _instanceId; }

private:
	uint64						_instanceId{};
	ComPtr<ID3D12Resource>		_buffer;
	D3D12_VERTEX_BUFFER_VIEW	_bufferView;

	uint32						_maxCount{};
	vector<InstancingParams>	_data;
};