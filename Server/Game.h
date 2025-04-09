#pragma once
#include "Player.h"
#include "Monster.h"
#include "Room.h"

class Game
{
	DECLARE_SINGLE(Game)

private:
	~Game() {}


public:
	void RegisterClient(int id);

	void ProcessPacket(const packet::Type type, void* packet);

	void Init();

private:
	std::array<Player, MAX_PLAYER> _playerList{};
	std::array<Room, MAX_ROOM>	_roomList{};
	std::array<Monster, MAX_MONSTER> _monsterList{};

	concurrency::concurrent_queue<int> _matchmakingQueue{};

};

