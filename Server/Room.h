#pragma once
#include "Player.h"

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

	void Update(const float delta);
	//void ClearMonsterPtrList() { _monsterPtrList.clear(); }
	//void AddMonsterPtr(std::shared_ptr<Monster>& monster_ptr) { _monsterPtrList.push_back(monster_ptr); }

	void ClearObjects();
	void AddObject(std::shared_ptr<Object> object);
	void InsertPlayers(const int idx, std::shared_ptr<Player>& players);

	void FireBullet(const int player_id);


	// client�鿡�� object�� ��ġ�� ����.
	void SendObjectsToClient();

	// getter and setter

	// void SetPlayerPtrList(const int id, std::shared_ptr<Player>& player_ptr);
	void SetRoomType(const RoomType room_type) { _roomType = room_type; }
	RoomType GetRoomType() const { return _roomType; }


	void SetRoomStatus(const RoomStatus room_status) { _roomStatus = room_status; }
	RoomStatus GetRoomStatus() const { return _roomStatus; }

	void SetRoomId(const int id) { _roomId = id; }

	// ��� �÷��̾ ���� �����ߴٰ� �� ���� ȣ��� �Լ�.
	// game���� ������ �ִ� object pool�� ��ȯ�� �־�� ��.
	void Clear();

private:

	// TODO:
	// �ϴ� �ӽ�. ���߿� ���⿡ client_id�� �����ϵ��� ����
	int _roomId{-1};
	
	// �÷��̾�� 3�� �����̴ϱ� monster ���꿡 ���ϵ��� array���� ����
	std::array<std::shared_ptr<Player>, 3>	_playerList{};


	// ��ü�� id�� ������ ��.
	std::atomic_int _idCount{ 0 };
	concurrency::concurrent_unordered_map<int, std::shared_ptr<Object>> _objects;

	RoomType			_roomType{ RoomType::NONE };
	RoomStatus			_roomStatus{ RoomStatus::Waiting };

};