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
	void InitRoom(int room_id);
	

	Vec2f GetPlayerPos(int player_id) { return _playerList[player_id].GetPos(); }
	void SetPlayerPos(int player_id, const Vec2f& pos) { _playerList[player_id].Move(pos); }

	void update();
	void Init();

private:

	// room 당 3명의 플레이어가 필수이므로
	// room * 3, room * 3 + 1, room * 3 + 2를 가지도록 했음.
	std::array<Player, MAX_PLAYER>		_playerList{};
	std::array<Room, MAX_ROOM>			_roomList{};

	// 몬스터가 없는 방도 있으므로 얘는 CAS를 통해서
	// 사용 가능한 몬스터를 꺼내 쓰는 방식으로 사용한다.
	std::array<Monster, MAX_MONSTER>	_monsterList{};

};

