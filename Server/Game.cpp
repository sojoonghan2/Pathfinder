#include "pch.h"
#include "Game.h"
#include "IOCP.h"
#include "Timer.h"

std::random_device rd_game;
std::default_random_engine dre_game{ rd_game() };
std::uniform_real_distribution<float> posDist{ -24.f, 24.f };

void Game::MovePlayer(int player_id, Vec2f& pos)
{
	_playerList[player_id].Move(pos);
}

void Game::InitRoom(int room_id)
{

	// ����� RUINS�� ����
	// �� ����
	_roomList[room_id].SetRoomType(RoomType::Ruin);
	_roomList[room_id].ClearMonsterPtrList();

	// �÷��̾� ����
	for (int i = 0; i < 3; ++i) {
		_roomList[room_id].SetPlayerPtrList(
			room_id + i,
			&_playerList[room_id * 3 + i]);

		_playerList[room_id * 3 + i].Move(posDist(dre_game), posDist(dre_game));
	}


	// ���� ����
	// �ϴ� �ӽ÷� 10����
	// todo:
	// �ӽ÷� ���Ͱ� ���涧���� ������. 
	std::array<int, 10> monster_ids{};
	while (true) {
		int count{ 0 };
		for (int i = 0; i < MAX_MONSTER; ++i) {
			if (false == _monsterList[i].GetRunning()) {
				if (_monsterList[i].TrySetRunning(true)) {
					monster_ids[count++] = i;
					if (10 == count) {
						break;
					}
				}
			}
		}
	}

	// ���� �߰�
	for (auto id : monster_ids) {
		_monsterList[id].Move(posDist(dre_game), posDist(dre_game));

		// TODO: 
		// ���� Ÿ���� �Է��ϸ� �ڵ����� ���� ������ �����ϵ���.
		// ���߿� lua����. 
		_monsterList[id].SetMonsterType(MonsterType::Crab);
		_monsterList[id].SetRoomId(room_id);
		_monsterList[id].SetSpeed(MONSTER_CRAB_SPEED_MPS);
		_roomList[room_id].AddMonsterPtr(&_monsterList[id]);
	}


}

void Game::Update(const float delta_time)
{
	
	// �ϴ� ���͸� ������Ʈ

	// ���� ������Ʈ
	for (auto& room : _roomList) {
		room.Update(delta_time);
	}
}

void Game::Init()
{
	
}
