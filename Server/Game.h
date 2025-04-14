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
	
	Monster& GetMonster(const int monster_id) { return _monsterList[monster_id]; }
	Room& GetRoom(const int room_id) { return _roomList[room_id]; }
	Player& GetPlayer(const int player_id) { return _playerList[player_id]; }

	void Update(const float delta_time);
	void Init();

private:

	// room �� 3���� �÷��̾ �ʼ��̹Ƿ�
	// room * 3, room * 3 + 1, room * 3 + 2�� �������� ����.
	std::array<Player, MAX_PLAYER>		_playerList{};
	std::array<Room, MAX_ROOM>			_roomList{};

	// ���Ͱ� ���� �浵 �����Ƿ� ��� CAS�� ���ؼ�
	// ��� ������ ���͸� ���� ���� ������� ����Ѵ�.
	std::array<Monster, MAX_MONSTER>	_monsterList{};

};

