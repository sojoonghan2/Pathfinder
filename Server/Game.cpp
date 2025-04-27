#include "pch.h"
#include "Game.h"
#include "IOCP.h"
#include "Timer.h"

std::random_device rd_game;
std::default_random_engine dre_game{ rd_game() };
std::uniform_real_distribution<float> posDist{ -24.f, 24.f };

std::uniform_real_distribution<float> speedDist{ 1.f, 3.f };


void Game::InitRoom(int room_id)
{

	// �� ����
	_roomList[room_id]->SetRoomStatus(RoomStatus::Preparing);
	_roomList[room_id]->ClearObjects();

	// TODO:
	// ���⸦ �������� �ްԲ� ����.
	_roomList[room_id]->SetRoomType(RoomType::Ruin);

	// �÷��̾� ����
	for (int i = 0; i < 3; ++i) {
		_roomList[room_id]->InsertPlayers(i, _playerList[room_id * 3 + i]);
		_playerList[room_id * 3 + i]->SetPlayerType(PlayerType::Dealer);
		_playerList[room_id * 3 + i]->SetPos(posDist(dre_game), posDist(dre_game));
	}


	// ���� ����
	// �ϴ� �ӽ÷� 10����
	// todo:
	// �ӽ÷� ���Ͱ� ���涧���� ������. 
	std::array<int, 10> monster_ids{};


	while (true) {
		int count{ 0 };
		for (int i = 0; i < MAX_MONSTER; ++i) {
			if (false == _monsterList[i]->GetRunning()) {
				if (_monsterList[i]->TrySetRunning(true)) {
					monster_ids[count++] = i;
					if (10 == count) {
						break;
					}
				}
			}
		}

		if (10 == count) {
			break;
		}
	}

	// ���� �߰�
	for (auto id : monster_ids) {
		_monsterList[id]->SetPos(posDist(dre_game), posDist(dre_game));

		// TODO: 
		// ���� Ÿ���� �Է��ϸ� �ڵ����� ���� ������ �����ϵ���.
		// ���߿� lua����. 
		_monsterList[id]->SetMonsterType(MonsterType::Crab);
		_monsterList[id]->SetRoomId(room_id);
		_monsterList[id]->SetSpeed(speedDist(dre_game));
		_roomList[room_id]->AddObject(_monsterList[id]);
	}

}

void Game::Update(const float delta_time)
{
	

	// �� ������Ʈ
	for (auto& room : _roomList) {
		room->Update(delta_time);
	}
}

void Game::Init()
{
	for (auto& player : _playerList) {
		player = std::make_shared<Player>();
	}

	for (auto& room : _roomList) {
		room = std::make_shared<Room>();
	}

	for (auto& monster : _monsterList) {
		monster = std::make_shared<Monster>();
	}
}
