#pragma once
#include "Message.h"

class MessageManager
{
	DECLARE_SINGLE(MessageManager)

public:

	using QueueType = std::queue<std::shared_ptr<Msg>>;

	void PushMessage(const int id, std::shared_ptr<Msg> msg);
	QueueType& GetMessageQueue(const int id);

private:
	std::unordered_map<int, QueueType> messageMap;
};
