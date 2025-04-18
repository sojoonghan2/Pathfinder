#include "pch.h"
#include "RoomInfo.h"

bool RoomInfo::CompareSetRunning(bool old_value, const bool new_value)
{
	return _isRunning.compare_exchange_strong(old_value, new_value);
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
