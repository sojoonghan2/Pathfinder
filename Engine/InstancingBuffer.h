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

	// �ν��Ͻ� ���� �ʱ�ȭ
	void Init(uint32 maxCount = 10);

	// ����� �ν��Ͻ� ������ �ʱ�ȭ, ���� ������ ������ ����
	void Clear();
	// �ν��Ͻ� ������ �߰�, �ν��Ͻ̿� �ʿ��� ����� �����ϴ� ����ü�� �޾� ���Ϳ� �߰�
	void AddData(InstancingParams& params);
	// ���Ϳ� ����� �ν��ͽ� ������ GPU ���۷� ����
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