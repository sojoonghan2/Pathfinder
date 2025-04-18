#include "pch.h"
#include "Game.h"
#include "IOCP.h"
#include "Timer.h"

std::random_device rd_game;
std::default_random_engine dre_game{ rd_game() };
std::uniform_real_distribution<float> posDist{ -24.f, 24.f };

std::uniform_real_distribution<float> speedDist{ 1.f, 3.f };

void Game::MovePlayer(int player_id, Vec2f& pos)
{
	_playerList[player_id].Move(pos);
}

void Game::InitRoom(int room_id)
{

	// 방 초기화
	_roomList[room_id].ClearMonsterPtrList();

	// 플레이어 설정
	for (int i = 0; i < 3; ++i) {
		_roomList[room_id].SetPlayerPtrList(
			room_id + i,
			&_playerList[room_id * 3 + i]);

		_playerList[room_id * 3 + i].Move(posDist(dre_game), posDist(dre_game));
	}


	// 몬스터 설정
	// 일단 임시로 10마리
	// todo:
	// 임시로 몬스터가 생길때까지 돌린다. 
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

		if (10 == count) {
			break;
		}
	}

	// 몬스터 추가
	for (auto id : monster_ids) {
		_monsterList[id].Move(posDist(dre_game), posDist(dre_game));

		// TODO: 
		// 몬스터 타입을 입력하면 자동으로 내부 설정이 가능하도록.
		// 나중엔 lua까지. 
		_monsterList[id].SetMonsterType(MonsterType::Crab);
		_monsterList[id].SetRoomId(room_id);
		_monsterList[id].SetSpeed(speedDist(dre_game));
		_roomList[room_id].AddMonsterPtr(&_monsterList[id]);
	}

	// 항상 방 타입을 설정하는 것은 마지막으로 한다.
	_roomList[room_id].SetRoomType(RoomType::Ruin);
}

void Game::Update(const float delta_time)
{
	
	// 일단 몬스터만 업데이트

	// 몬스터 업데이트
	for (auto& room : _roomList) {
		if (room.GetRoomType() == RoomType::None) continue;
		room.Update(delta_time);
	}
}

void Game::Init()
{
	
}
