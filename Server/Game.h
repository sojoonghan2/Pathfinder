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

	// room �� 3���� �÷��̾ �ʼ��̹Ƿ�
	// room * 3, room * 3 + 1, room * 3 + 2�� �������� ����.
	std::array<std::shared_ptr<Player>, MAX_PLAYER>		_playerList{};
	std::array<std::shared_ptr<Room>, MAX_ROOM>			_roomList{};

	// ���Ͱ� ���� �浵 �����Ƿ� ��� CAS�� ���ؼ�
	// ��� ������ ���͸� ���� ���� ������� ����Ѵ�.
	std::array<std::shared_ptr<Monster>, MAX_MONSTER>	_monsterList{};

	Timer _deltaTimer;
	Timer _sendTimer;
};

