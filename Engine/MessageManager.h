#pragma once
#include "Message.h"

enum
{
	ID_SCENE_START = 100000,
	ID_RUINS_SCENE,
};

/*
�� ��script������ awake() �ܰ迡��
RegisterObject(ObejctType type, int object_id)�� ȣ���ؼ�
���� ��ü�� messageManager�� ���

�̶�, �ڽ��� Ÿ���� ���� ��� ��Ʈ��ũid�� ����� �� �� ����.

unordered_map<objectType, std::queue<int(object_id)>> �� �� ��ü�� ���

network_id�� �� ��ü�� ������ �Ǿ��� ��
����ִ� ObjectType Ÿ���� ��ü�� �ο����ش�.

�̰� unordered_map<network_id message_id>�� ����

������ ����� ������ 2���� ����� �ٽ� 1�� �߰�.

**�߿� �� ���� ��ȯ�� �� MessageManager���� ���� ���� ��� �����ִ� �۾��� �ʿ���.
*/

class MessageManager
{
	DECLARE_SINGLE(MessageManager)

public:

	using MsgQueueType = std::queue<std::shared_ptr<Msg>>;

	void PushMessage(const int32 object_id, std::shared_ptr<Msg> msg);
	MsgQueueType& GetMessageQueue(const uint32 object_id);

	bool PushMessageByNetworkId(const int network_id, std::shared_ptr<Msg> msg);


	void RegisterObject(const ObjectType object_type, const uint32 object_id);
	void RegisterScene(const int network_id, const uint32 scene_id);
	
	bool AllocNetworkObject(const ObjectType object_type, const int network_id);

	bool FindNetworkObject(const int network_id) const;

	void DeleteNetworkObject(const int network_id);

	void Clear();
	
private:
	// message_id, message_queue
	std::unordered_map<uint32, MsgQueueType> _messageQueueHash;

	// for network

	// ��ϵ� ��ü�� ť
	std::unordered_map<ObjectType, std::queue<uint32>> _objectQueueHash;

	// ��Ʈ��ũ ���̵�� Ŭ���̾�Ʈ ��ü id�� ��� �ؽ�
	std::unordered_map<int, uint32> _networkToClientHash;
};
