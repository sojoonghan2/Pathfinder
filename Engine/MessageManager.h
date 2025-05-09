#pragma once
#include "Message.h"

enum
{
	ID_SCENE_START = 100000,
	ID_RUINS_SCENE,
};

/*
각 “script”에서 awake() 단계에서
RegisterObject(ObejctType type, int object_id)를 호출해서
본인 객체를 messageManager에 등록

이때, 자신의 타입을 같이 줘야 네트워크id에 등록을 할 수 있음.

unordered_map<objectType, std::queue<int(object_id)>> 에 빈 객체를 등록

network_id로 새 객체가 생성이 되었을 때
비어있는 ObjectType 타입의 객체를 부여해준다.

이걸 unordered_map<network_id message_id>에 저장

지우라는 명령이 들어오면 2에서 지우고 다시 1에 추가.

**중요 각 씬이 전환될 때 MessageManager에게 기존 맵을 모두 지워주는 작업이 필요함.
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

	// 등록된 객체의 큐
	std::unordered_map<ObjectType, std::queue<uint32>> _objectQueueHash;

	// 네트워크 아이디로 클라이언트 객체 id를 얻는 해시
	std::unordered_map<int, uint32> _networkToClientHash;
};
