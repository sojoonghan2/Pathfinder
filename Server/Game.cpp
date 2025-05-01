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
	_roomList[room_id]->SetRoomId(room_id);

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
	std::array<std::shared_ptr<Monster>, 10> monsters{};
	for (auto& monster : monsters) {
		std::shared_ptr<Object> obj;
		if (not _objectPoolHash[ObjectType::Monster].try_pop(obj)) {
			monster = std::shared_ptr<Monster>();
			continue;
		}
		monster = std::dynamic_pointer_cast<Monster, Object>(obj);
	}
	
	// 몬스터 추가
	for (auto& monster: monsters) {
		monster->SetPos(posDist(dre_game), posDist(dre_game));

		// TODO: 
		// 몬스터 타입을 입력하면 자동으로 내부 설정이 가능하도록.
		// 나중엔 lua까지.
		monster->SetMonsterType(MonsterType::Crab);
		monster->SetRoomId(room_id);
		monster->SetSpeed(speedDist(dre_game));
		_roomList[room_id]->AddObject(monster);
	}

}

std::shared_ptr<Player> Game::GetPlayer(const ClientIdInfo& id_info)
{
	return _playerList.at(id_info.roomId * 3 + id_info.playerId);
}

void Game::Update()
{
	// 방 업데이트
	_deltaTimer.updateDeltaTime();
	auto delta = _deltaTimer.getDeltaTimeSeconds();
	for (auto& room : _roomList) {
		room->Update(delta);
	}


	if (_sendTimer.PeekDeltaTime() > MOVE_PACKET_TIME_MS) {
		_sendTimer.updateDeltaTime();
		// 플레이어에게 send


		for (auto& room : _roomList) {
			room->SendObjectsToClient();
		}
	}
}

void Game::Init()
{
	// player init
	for (auto& player : _playerList) {
		player = std::make_shared<Player>();
	}

	// room init
	for (auto& room : _roomList) {
		room = std::make_shared<Room>();
	}

	// monster init
	_objectPoolHash[ObjectType::Monster].clear();

	for (int i = 0; i < MAX_MONSTER; ++i) {
		_objectPoolHash[ObjectType::Monster].push(
			std::make_shared<Monster>()
		);
	}

	// 나중엔 총알도 넣을 예정.

}
