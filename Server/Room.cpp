#include "pch.h"
#include "Room.h"

void Room::InsertPlayer(const int idx, const int player_id)
{
	_playerIdList[idx] = player_id;
}
