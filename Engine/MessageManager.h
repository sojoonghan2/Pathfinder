#pragma once

enum class ObjectId
{
	NONE,
	PLAYER
};


class MessageManager
{
	DECLARE_SINGLE(MessageManager)

public:

	using QueueType = std::queue<std::pair<float, float>>;

	// temp
	void InsertMessage(ObjectId id, float x, float y);
	QueueType& GetMessageQueue(ObjectId id);

private:
	std::unordered_map<ObjectId, QueueType> messageMap;
};

