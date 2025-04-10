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

	// room �� 3���� �÷��̾ �ʼ��̹Ƿ�
	// room * 3, room * 3 + 1, room * 3 + 2�� �������� ����.
	std::array<Player, MAX_PLAYER>		_playerList{};
	std::array<Room, MAX_ROOM>			_roomList{};

	// ���Ͱ� ���� �浵 �����Ƿ� ��� CAS�� ���ؼ�
	// ��� ������ ���͸� ���� ���� ������� ����Ѵ�.
	std::array<Monster, MAX_MONSTER>	_monsterList{};

};

