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
	//void ClearMonsterPtrList() { _monsterPtrList.clear(); }
	//void AddMonsterPtr(std::shared_ptr<Monster>& monster_ptr) { _monsterPtrList.push_back(monster_ptr); }

	void ClearObjects();
	void AddObject(std::shared_ptr<Object> object);
	void InsertPlayers(const int idx, std::shared_ptr<Player>& players);

	void SyncObjects();

	// getter and setter

	// void SetPlayerPtrList(const int id, std::shared_ptr<Player>& player_ptr);
	void SetRoomType(const RoomType room_type) { _roomType = room_type; }
	void SetRoomStatus(const RoomStatus room_status) { _roomStatus = room_status; }

	//std::vector<std::shared_ptr<Monster>>& GetMonsterPtrList() { return _monsterPtrList; }

	RoomStatus GetRoomStatus() const { return _roomStatus; }
	RoomType GetRoomType() const { return _roomType; }

private:
	
	// �÷��̾�� 3�� �����̴ϱ� monster ���꿡 ���ϵ��� array���� ����
	std::array<std::shared_ptr<Player>, 3>	_playerList{};


	// ��ü�� id�� ������ ��.
	std::atomic_int _idCount{ 0 };
	concurrency::concurrent_unordered_map<int, std::shared_ptr<Object>> _writerObjects;
	concurrency::concurrent_unordered_map<int, std::shared_ptr<Object>> _readerObjects;


	RoomType			_roomType{ RoomType::None };
	RoomStatus			_roomStatus{ RoomStatus::Waiting };
};