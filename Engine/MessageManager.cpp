#include "pch.h"
#include "MessageManager.h"



void MessageManager::InsertMessage(ObjectId id, float x, float y)
{
	messageMap[id].push(std::make_pair(x, y));
}

MessageManager::QueueType& MessageManager::GetMessageQueue(ObjectId id)
{
	return messageMap[id];
}
