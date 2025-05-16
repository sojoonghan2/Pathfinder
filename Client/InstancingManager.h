#pragma once
#include "Buffer.h"

class GameObject;

// �ν��Ͻ� ����ȭ�� ���� ������ �Ŵ���
// ������ �޽�/��Ƽ������ �����ϴ� ��ü���� �ϳ��� Draw�� ����
class InstancingManager
{
	DECLARE_SINGLE(InstancingManager);

public:
	// ������ ������ ȣ��: ��� ������Ʈ ������
	void Render(vector<shared_ptr<GameObject>>& gameObjects);

	// �ν��Ͻ� ���� �ʱ�ȭ (������ ���� ��)
	void ClearBuffer();

	// ���� �ʱ�ȭ (��: �� ��ȯ ��)
	void Clear();

private:
	// Ư�� �ν��Ͻ� ID�� ���� ������ ����
	void AddParam(uint64 instanceId, const InstancingParams& data);

private:
	// instanceId(�޽�+��Ƽ���� ���� �ؽ�) �� �ν��Ͻ� ���� ����
	map<uint64, shared_ptr<InstancingBuffer>> _buffers;
};
