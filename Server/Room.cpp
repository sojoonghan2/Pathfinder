#include "pch.h"
#include "Room.h"

bool Room::TrySetRunning(const bool running)
{
	bool expected = !running;
	return _isRunning.compare_exchange_strong(expected, running);
}

bool Room::GetRunning() const
{
	return _isRunning.load();
}