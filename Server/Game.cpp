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

	// 방 설정
	_roomList[room_id]->SetRoomStatus(RoomStatus::Preparing);
	_roomList[room_id]->ClearObjects();

	// TODO:
	// 여기를 랜덤으로 받게끔 설정.
	_roomList[room_id]->SetRoomType(RoomType::Ruin);

	// 플레이어 설정
	for (int i = 0; i < 3; ++i) {
		_roomList[room_id]->InsertPlayers(i, _playerList[room_id * 3 + i]);
		_playerList[room_id * 3 + i]->SetPlayerType(PlayerType::Dealer);
		_playerList[room_id * 3 + i]->SetPos(posDist(dre_game), posDist(dre_game));
	}


	// 몬스터 설정
	// 일단 임시로 10마리
	// todo:
	// 임시로 몬스터가 생길때까지 돌린다. 
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

	// 몬스터 추가
	for (auto id : monster_ids) {
		_monsterList[id]->SetPos(posDist(dre_game), posDist(dre_game));

		// TODO: 
		// 몬스터 타입을 입력하면 자동으로 내부 설정이 가능하도록.
		// 나중엔 lua까지. 
		_monsterList[id]->SetMonsterType(MonsterType::Crab);
		_monsterList[id]->SetRoomId(room_id);
		_monsterList[id]->SetSpeed(speedDist(dre_game));
		_roomList[room_id]->AddObject(_monsterList[id]);
	}

}

void Game::Update(const float delta_time)
{
	

	// 방 업데이트
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
