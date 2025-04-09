#include "pch.h"
#include "RoomInfo.h"

bool RoomInfo::TrySetRunning(const bool running)
{
	bool expected = not running;
	return _isRunning.compare_exchange_strong(expected, running);
}

bool RoomInfo::GetRunning() const
{
	return _isRunning.load();
}