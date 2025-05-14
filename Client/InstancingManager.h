#pragma once
#include "Buffer.h"

class GameObject;

class InstancingManager
{
	DECLARE_SINGLE(InstancingManager);

public:
	void Render(vector<shared_ptr<GameObject>>& gameObjects);

	void ClearBuffer();
	void Clear() { _buffers.clear(); }

private:
	// Ư�� �ν��Ͻ� ID�� ���� �Ķ���� �߰�
	void AddParam(uint64 instanceId, InstancingParams& data);

private:
	// �� �ν��Ͻ� ID�� ���� ���õ� �ν��Ͻ� ���۸� �����ϴ� ��
	map<uint64/*instanceId*/, shared_ptr<InstancingBuffer>> _buffers;
};
