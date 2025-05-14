#include "pch.h"
#include "MessageManager.h"

void MessageManager::PushMessage(const int32 object_id, std::shared_ptr<Msg> msg)
{
	_messageQueueHash[object_id].push(msg);
}

MessageManager::MsgQueueType& MessageManager::GetMessageQueue(const uint32 object_id)
{
	return _messageQueueHash[object_id];
}

bool MessageManager::PushMessageByNetworkId(const int network_id, std::shared_ptr<Msg> msg)
{
	if (not _networkToClientHash.contains(network_id)) {
		return false;
	}

	const auto object_id{ _networkToClientHash[network_id] };
	_messageQueueHash[object_id].push(msg);
	return true;
}

void MessageManager::RegisterObject(const ObjectType object_type, const uint32 object_id)
{
	_objectQueueHash[object_type].push(object_id);
}

void MessageManager::RegisterStaticObject(const int network_id, const uint32 scene_id)
{
	_networkToClientHash[network_id] = scene_id;
}

bool MessageManager::AllocNetworkObject(const ObjectType object_type, const int network_id)
{
	if (_objectQueueHash[object_type].empty()) {
		// 현재 생성 가능한 객체가 없음.
		return false;
	}

	const auto object_id{ _objectQueueHash[object_type].front() };
	_objectQueueHash[object_type].pop();
	_networkToClientHash[network_id] = object_id;
	return true;
}

bool MessageManager::FindNetworkObject(const int network_id) const
{
	return _networkToClientHash.contains(network_id);
}

void MessageManager::DeleteNetworkObject(const int network_id)
{
	if (not _networkToClientHash.contains(network_id)) {
		return;
	}

	auto object_id{ _networkToClientHash[network_id] };


	_networkToClientHash.erase(network_id);
	PushMessage(object_id, std::make_shared<Msg>(MsgType::REGISTER));
	
	
}

void MessageManager::Clear()
{
	_messageQueueHash.clear();
	_objectQueueHash.clear();
	_networkToClientHash.clear();
}


