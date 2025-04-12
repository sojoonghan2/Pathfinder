#pragma once
#include "Player.h"
#include "Monster.h"

enum class RoomType : unsigned char
{
	None = 0,
	Ruin,
	Factory,
	Ristrict,
	Falling,
	Lucky
};

class Room
{
public:

	void Update(const float delta_time);

	void SetPlayerPtr(const int room_id);
	std::array<int, 3> GetPlayerIdList() const;
	void ClearMonsterPtrList() { _monsterPtrList.clear(); }
	void AddMonsterPtr(Monster* monster_ptr) { _monsterPtrList.push_back(monster_ptr); }
	
	void SetRoomType(const RoomType room_type) { _roomType = room_type; }

private:
	
	std::array<Player*, 3>	_playerPtrList{};
	std::vector<Monster*>	_monsterPtrList{};

	RoomType			_roomType{ RoomType::None };
};