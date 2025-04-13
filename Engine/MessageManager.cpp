#include "pch.h"
#include "MessageManager.h"

void MessageManager::InsertMessage(int id, float x, float y)
{
	messageMap[id].push(std::make_pair(x, y));
}

MessageManager::QueueType& MessageManager::GetMessageQueue(int id)
{
	return messageMap[id];
}