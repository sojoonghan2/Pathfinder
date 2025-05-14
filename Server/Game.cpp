#include "pch.h"
#include "Game.h"
#include "IOCP.h"
#include "Timer.h"

#include "Monster.h"
#include "Bullet.h"


std::random_device rd_game;
std::default_random_engine dre_game{ rd_game() };
std::uniform_real_distribution<float> posDist{ -24.f, 24.f };

std::uniform_real_distribution<float> speedDist{ 1.f, 3.f };


void Pathfinder::InitRoom(int room_id)
{

	// 방 설정
	_roomList[room_id]->SetRoomStatus(RoomStatus::Preparing);
	_roomList[room_id]->ClearObjects();
	_roomList[room_id]->SetRoomId(room_id);

	// TODO:
	// 여기를 랜덤으로 받게끔 설정.
	_roomList[room_id]->SetRoomType(RoomType::RUIN);

	// 플레이어 설정
	for (int i = 0; i < 3; ++i) {
		_roomList[room_id]->InsertPlayers(i, _playerList[room_id * 3 + i]);
		_playerList[room_id * 3 + i]->SetPlayerType(PlayerType::Dealer);
		_playerList[room_id * 3 + i]->SetPos(posDist(dre_game), posDist(dre_game));
	}


	// 몬스터 설정
	
	// 몬스터 꺼내오기
	// 일단 임시로 10마리. 방타입에 따라 몬스터 양이 달라져야 함.
	std::array<std::shared_ptr<Monster>, 10> monsters{};
	for (auto& monster : monsters) {
		auto obj{ GetObjectFromPool(ObjectType::MONSTER) };
		monster = std::dynamic_pointer_cast<Monster, Object>(obj);
	}
	
	// 몬스터 추가
	for (auto& monster: monsters) {

		// 일단 임시로 게
		monster->InitMonster(
			MonsterType::CRAB,
			Vec2f{ posDist(dre_game), posDist(dre_game) });
		_roomList[room_id]->AddObject(monster);
	}

}

std::shared_ptr<Player> Pathfinder::GetPlayer(const ClientIdInfo& id_info)
{
	return _playerList.at(id_info.roomId * 3 + id_info.playerId);
}

std::shared_ptr<Object> Pathfinder::GetObjectFromPool(const ObjectType type)
{
	std::shared_ptr<Object> obj;

	// 실패하면 새 객체를 만들어 주어야 한다.
	if (not _objectPoolHash[type].try_pop(obj)) {
		switch (type) {
		case ObjectType::MONSTER:
		{
			obj = std::make_shared<Monster>();
		}
		break;
		case ObjectType::BULLET:
		{
			obj = std::make_shared<Bullet>();
		}
		break;
		default:
			break;
		}
	}
	return obj;
}

void Pathfinder::Update()
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

void Pathfinder::Init()
{
	// player init
	for (auto& player : _playerList) {
		player = std::make_shared<Player>();
	}

	// room init
	for (auto& room : _roomList) {
		room = std::make_shared<Room>();
	}


	_objectPoolHash[ObjectType::MONSTER].clear();

	for (int i = 0; i < MAX_MONSTER; ++i) {
		_objectPoolHash[ObjectType::MONSTER].push(
			std::make_shared<Monster>()
		);
	}

	_objectPoolHash[ObjectType::BULLET].clear();

	for (int i = 0; i < MAX_BULLET; ++i) {
		_objectPoolHash[ObjectType::BULLET].push(
			std::make_shared<Bullet>()
		);
	}

}
