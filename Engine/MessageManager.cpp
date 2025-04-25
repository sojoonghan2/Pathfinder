#include "pch.h"
#include "MessageManager.h"

void MessageManager::PushMessage(const int id, std::shared_ptr<Msg> msg)
{
	messageMap[id].push(msg);
}

MessageManager::QueueType& MessageManager::GetMessageQueue(const int id)
{
	return messageMap[id];
}