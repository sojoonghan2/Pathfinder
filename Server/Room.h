#pragma once
#include "Player.h"
#include "Monster.h"

enum class RoomStatus : unsigned char
{
	None = 0,

	// ���� �Ҵ��� ���� ���� ����
	Waiting,

	// ��, �÷��̾��� �غ�ϷḦ ��ٸ��� �ִ� ����
	Preparing,

	// ���� �۵����� ����
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

	// �̰� ����?
	std::vector<Monster*>& GetMonsterPtrList() { return _monsterPtrList; }

	RoomStatus GetRoomStatus() const { return _roomStatus; }
	RoomType GetRoomType() const { return _roomType; }

private:
	
	// TODO: ���⸦ shared_ptr��
	std::array<Player*, 3>	_playerPtrList{};
	std::vector<Monster*>	_monsterPtrList{};

	RoomType			_roomType{ RoomType::None };
	RoomStatus			_roomStatus{ RoomStatus::Waiting };
};