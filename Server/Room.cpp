#include "pch.h"
#include "Room.h"

void Room::SetPlayerIdList(const int room_id)
{
	_playerIdList[0] = room_id * 3;
	_playerIdList[1] = room_id * 3 + 1;
	_playerIdList[2] = room_id * 3 + 2;
}

std::array<int, 3> Room::GetPlayerIdList() const
{
	return _playerIdList;
}
