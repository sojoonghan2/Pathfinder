#pragma once
#include "Player.h"
#include "Room.h"
#include "Timer.h"
#include "Obstacle.h"

// object handler ���ҵ� ���ش�.
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

	// room �� 3���� �÷��̾ �ʼ��̹Ƿ�
	// room * 3, room * 3 + 1, room * 3 + 2�� �������� ����.
	// ���� �ٸ� ������ access�ϹǷ� data race�� �ƴ�.
	std::array<std::shared_ptr<Player>, MAX_PLAYER>		_playerList{};

	// data race. room ���������� ���ÿ� �����ص� ������ ������ �ؾ� ��.
	std::array<std::shared_ptr<Room>, MAX_ROOM>			_roomList{};


	using ObjectPool = concurrency::concurrent_queue<std::shared_ptr<Object>>;

	//std::array<std::shared_ptr<Monster>, MAX_MONSTER>	_monsterList{};
	// �� ���� ��ü�� ����Ǿ� �ִ� Queue. �ʿ��Ҷ����� ������ �� room�� �����Ѵ�.
	// std::unordered_map ��ü�� ������ �̷������ �����Ƿ� no data race
	std::unordered_map<ObjectType, ObjectPool> _objectPoolHash;


	using ObstacleVec = std::vector<std::shared_ptr<Obstacle>>;

	// �� ���� ��ֹ��� ����Ǿ� �ִ� �ڷᱸ��.
	// read only. ������ ��� data race�� �ƴ�.
	// 4 TODO: init �� ���⿡ ��ֹ��� �ִ´�.
	// ��ü �ڵ鷯�� ������ ���� �� ���ҵ�?
	std::unordered_map<RoomType, ObstacleVec> _staticObstacleVec;

	Timer _deltaTimer;
	Timer _sendTimer;
};

