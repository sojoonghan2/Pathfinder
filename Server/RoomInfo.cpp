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

void RoomInfo::InsertClient(const int idx, const int client_id)
{
	_clientIdList[idx] = client_id;
}

std::array<int, 3> RoomInfo::GetClientIdList() const
{
	return _clientIdList;
}
