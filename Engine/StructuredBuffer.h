#pragma once

class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();

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

private:
	void CopyInitialData(uint64 bufferSize, void* initialData);

private:
	ComPtr<ID3D12Resource>			_buffer;
	ComPtr<ID3D12DescriptorHeap>	_srvHeap;
	ComPtr<ID3D12DescriptorHeap>	_uavHeap;

	uint32						_elementSize = 0;
	uint32						_elementCount = 0;
	D3D12_RESOURCE_STATES		_resourceState = {};

private:
	D3D12_CPU_DESCRIPTOR_HANDLE _srvHeapBegin = {};
	D3D12_CPU_DESCRIPTOR_HANDLE _uavHeapBegin = {};
};
