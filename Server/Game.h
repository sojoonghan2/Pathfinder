#pragma once
#include "Player.h"
#include "Monster.h"
#include "Room.h"
#include "Timer.h"

class Game
{
	DECLARE_SINGLE(Game)

private:
	~Game() {}


public:

	void InitRoom(int room_id);
	
	std::shared_ptr<Monster> GetMonster(const int monster_id) { return _monsterList.at(monster_id); }
	std::shared_ptr<Room> GetRoom(const int room_id) { return _roomList.at(room_id); }
	std::shared_ptr<Player> GetPlayer(const int player_id) { return _playerList.at(player_id); }
	std::shared_ptr<Player> GetPlayer(const ClientIdInfo& id_info);

	void Update();
	void Init();

private:

	// room 당 3명의 플레이어가 필수이므로
	// room * 3, room * 3 + 1, room * 3 + 2를 가지도록 했음.
	std::array<std::shared_ptr<Player>, MAX_PLAYER>		_playerList{};
	std::array<std::shared_ptr<Room>, MAX_ROOM>			_roomList{};

	// 몬스터가 없는 방도 있으므로 얘는 CAS를 통해서
	// 사용 가능한 몬스터를 꺼내 쓰는 방식으로 사용한다.
	std::array<std::shared_ptr<Monster>, MAX_MONSTER>	_monsterList{};

	Timer _deltaTimer;
	Timer _sendTimer;
};

