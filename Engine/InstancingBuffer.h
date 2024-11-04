#pragma once

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

public:
	// Getter
	uint32						GetCount() { return static_cast<uint32>(_data.size()); }
	ComPtr<ID3D12Resource>		GetBuffer() { return _buffer; }
	D3D12_VERTEX_BUFFER_VIEW	GetBufferView() { return _bufferView; }

	void	SetID(uint64 instanceId) { _instanceId = instanceId; }
	uint64	GetID() { return _instanceId; }

private:
	uint64						_instanceId = 0;
	ComPtr<ID3D12Resource>		_buffer;
	D3D12_VERTEX_BUFFER_VIEW	_bufferView;

	uint32						_maxCount = 0;
	vector<InstancingParams>	_data;
};