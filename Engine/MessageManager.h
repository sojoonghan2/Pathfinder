#pragma once

class MessageManager
{
	DECLARE_SINGLE(MessageManager)

public:

	using QueueType = std::queue<std::pair<float, float>>;

	// temp
	void InsertMessage(int id, float x, float y);
	QueueType& GetMessageQueue(int id);

private:
	std::unordered_map<int, QueueType> messageMap;
};

