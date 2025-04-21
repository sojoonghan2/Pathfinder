#pragma once
#include "Player.h"
#include "Monster.h"

enum class RoomStatus : unsigned char
{
	None = 0,

	// 방이 할당이 되지 않은 상태
	Waiting,

	// 방, 플레이어의 준비완료를 기다리고 있는 상태
	Preparing,

	// 현재 작동중인 상태
	Running,

};

class Room
{
public:

	void Update(const float delta_time);
	void ClearMonsterPtrList() { _monsterPtrList.clear(); }
	void AddMonsterPtr(Monster* monster_ptr) { _monsterPtrList.push_back(monster_ptr); }

	// getter and setter

	void SetPlayerPtrList(const int id, Player* player_ptr);
	void SetRoomType(const RoomType room_type) { _roomType = room_type; }
	void SetRoomStatus(const RoomStatus room_status) { _roomStatus = room_status; }

	// 이게 맞음?
	std::vector<Monster*>& GetMonsterPtrList() { return _monsterPtrList; }

	RoomStatus GetRoomStatus() const { return _roomStatus; }
	RoomType GetRoomType() const { return _roomType; }

private:
	
	// TODO: 여기를 shared_ptr로
	std::array<Player*, 3>	_playerPtrList{};
	std::vector<Monster*>	_monsterPtrList{};

	RoomType			_roomType{ RoomType::None };
	RoomStatus			_roomStatus{ RoomStatus::Waiting };
};