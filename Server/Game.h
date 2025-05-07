#pragma once
#include "Player.h"
#include "Room.h"
#include "Timer.h"
#include "Obstacle.h"

// object handler 역할도 해준다.
class Game
{
	DECLARE_SINGLE(Game)

private:
	~Game() {}


public:

	void InitRoom(int room_id);

	std::shared_ptr<Room> GetRoom(const int room_id) { return _roomList.at(room_id); }
	std::shared_ptr<Player> GetPlayer(const int player_id) { return _playerList.at(player_id); }
	std::shared_ptr<Player> GetPlayer(const ClientIdInfo& id_info);

	std::shared_ptr<Object> GetObjectFromPool(const ObjectType type);

	void Update();
	void Init();

private:

	// room 당 3명의 플레이어가 필수이므로
	// room * 3, room * 3 + 1, room * 3 + 2를 가지도록 했음.
	// 각각 다른 영역을 access하므로 data race가 아님.
	std::array<std::shared_ptr<Player>, MAX_PLAYER>		_playerList{};

	// data race. room 내부적으로 동시에 실행해도 문제가 없도록 해야 함.
	std::array<std::shared_ptr<Room>, MAX_ROOM>			_roomList{};


	using ObjectPool = concurrency::concurrent_queue<std::shared_ptr<Object>>;

	//std::array<std::shared_ptr<Monster>, MAX_MONSTER>	_monsterList{};
	// 각 동적 객체가 저장되어 있는 Queue. 필요할때마다 꺼내서 각 room에 저장한다.
	// std::unordered_map 자체는 수정이 이루어지지 않으므로 no data race
	std::unordered_map<ObjectType, ObjectPool> _objectPoolHash;


	using ObstacleVec = std::vector<std::shared_ptr<Obstacle>>;

	// 각 정적 장애물이 저장되어 있는 자료구조.
	// read only. 수정이 없어서 data race가 아님.
	// 4 TODO: init 시 여기에 장애물을 넣는다.
	// 객체 핸들러가 있으면 조금 더 편할듯?
	std::unordered_map<RoomType, ObstacleVec> _staticObstacleVec;

	Timer _deltaTimer;
	Timer _sendTimer;
};

