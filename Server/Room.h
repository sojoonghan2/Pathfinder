#pragma once

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
	void SetPlayerIdList(const int room_id);
	std::array<int, 3> GetPlayerIdList() const;
	void ClearMonsterIdList() { _monsterIdList.clear(); }
	void AddMonsterId(const int monster_id) { _monsterIdList.push_back(monster_id); }
	
	void SetRoomType(const RoomType room_type) { _roomType = room_type; }

private:
	std::array<int, 3>	_playerIdList{};
	std::vector<int>	_monsterIdList{};

	RoomType			_roomType{ RoomType::None };
};