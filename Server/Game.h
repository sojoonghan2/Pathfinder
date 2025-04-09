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

	void MovePlayer(int player_id, Vec2f& pos);
	bool GetNewPlayerIds(std::array<int, 3>& ids) const;
	
	// TODO: 몬스터를 위한 update 필요

	void Init();

private:
	std::array<Player, MAX_PLAYER>		_playerList{};
	std::array<Room, MAX_ROOM>			_roomList{};
	std::array<Monster, MAX_MONSTER>	_monsterList{};

};

