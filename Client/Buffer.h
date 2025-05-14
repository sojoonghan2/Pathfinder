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

	// �׷��Ƚ� ���������ο��� �ش� ���۸� SRV�� ���ε�
	void PushGraphicsData(SRV_REGISTER reg);
	// ��ǻ�� ���������ο��� �ش� ���۸� SRV�� ���ε�
	void PushComputeSRVData(SRV_REGISTER reg);
	// ��ǻ�� ���������ο��� �ش� ���۸� UAV�� ���ε�
	void PushComputeUAVData(UAV_REGISTER reg);

	ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAV() { return _uavHeap; }

	// ������ ���� ����
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

	// �ʱ�ȭ
	void Init(CBV_REGISTER reg, uint32 size, uint32 count);

	// ��� ���۸� ���(�ε����� �ʱ�ȭ�ϰų� �޸� �缳��)
	void Clear();
	// �׷��Ƚ� ���������ο��� ����� �������� ������ ��� ���ۿ� ����
	void PushGraphicsData(void* buffer, uint32 size);
	// �׷��Ƚ� ������������ ���� �����͸� ��� ���ۿ� ����
	void SetGraphicsGlobalData(void* buffer, uint32 size);
	// ��ǻƮ ���������ο��� ����� ������ ���� ��� ���ۿ� ����
	void PushComputeData(void* buffer, uint32 size);

	// ������ �ε����� �ش��ϴ� ��� ������ GPU ���� �ּ� ��ȯ
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);
	// ��� ���ۿ� ���� CPU �� �ڵ� ��ȯ
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);

private:
	// ��� ���� ���ҽ� ����
	void CreateBuffer();
	// ��� ���� �� ����
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

	// �ν��Ͻ� ���� �ʱ�ȭ
	void Init(uint32 maxCount = 10);

	// ����� �ν��Ͻ� ������ �ʱ�ȭ, ���� ������ ������ ����
	void Clear();
	// �ν��Ͻ� ������ �߰�, �ν��Ͻ̿� �ʿ��� ����� �����ϴ� ����ü�� �޾� ���Ϳ� �߰�
	void AddData(InstancingParams& params);
	// ���Ϳ� ����� �ν��ͽ� ������ GPU ���۷� ����
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