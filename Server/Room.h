#pragma once
#include "Player.h"

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

	void Update(const float delta);
	//void ClearMonsterPtrList() { _monsterPtrList.clear(); }
	//void AddMonsterPtr(std::shared_ptr<Monster>& monster_ptr) { _monsterPtrList.push_back(monster_ptr); }

	void ClearObjects();
	void AddObject(std::shared_ptr<Object> object);
	void InsertPlayers(const int idx, std::shared_ptr<Player>& players);


	// client들에게 object의 위치를 보냄.
	void SendObjectsToClient();

	void SyncObjects();

	// getter and setter

	// void SetPlayerPtrList(const int id, std::shared_ptr<Player>& player_ptr);
	void SetRoomType(const RoomType room_type) { _roomType = room_type; }
	RoomType GetRoomType() const { return _roomType; }


	void SetRoomStatus(const RoomStatus room_status) { _roomStatus = room_status; }
	RoomStatus GetRoomStatus() const { return _roomStatus; }

	void SetRoomId(const int id) { _roomId = id; }

private:

	// TODO:
	// 일단 임시. 나중에 여기에 client_id를 저장하도록 변경
	int _roomId{-1};
	
	// 플레이어는 3명 고정이니까 monster 연산에 편하도록 array에도 저장
	std::array<std::shared_ptr<Player>, 3>	_playerList{};


	// 객체가 id를 가져야 함.
	std::atomic_int _idCount{ 0 };
	concurrency::concurrent_unordered_map<int, std::shared_ptr<Object>> _writerObjects;
	std::unordered_map<int, std::shared_ptr<Object>> _readerObjects;


	RoomType			_roomType{ RoomType::None };
	RoomStatus			_roomStatus{ RoomStatus::Waiting };

};