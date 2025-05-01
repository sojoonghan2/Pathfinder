#include "pch.h"
#include "Room.h"
#include "IOCP.h"

void Room::Update(const float delta)
{
	// 활성화 되어 있는 방만 update한다.
	if (RoomType::None == _roomType ||
		RoomStatus::Running != _roomStatus) {
		return;
	}

	// 순회 중간에 객체가 들어오는 현상 방지
	SyncObjects();

	// TODO: 충돌 확인
	// readerObjects 간 충돌 체크
	for (auto iter{ _readerObjects.begin() }; iter != _readerObjects.end(); ++iter) {
		auto iter2{ iter };
		++iter2;
		for (; iter2 != _readerObjects.end(); ++iter2) {
			if (iter->second->CheckCollision(iter2->second)) {
				// 확인 완료
			}
		}
	};

	for (auto& [id, object] : _readerObjects) {

		// 몬스터
		if (ObjectType::Monster == object->GetObjectType()) {

			std::shared_ptr<Player> player = nullptr;
			float min_distance{ std::numeric_limits<float>::max() };
			auto monster_pos{ object->GetPos() };

			// 가장 가까운 플레이어 찾기
			for (int i = 0; i < 3; i++) {
				auto pos{ _playerList[i]->GetPos() };
				auto distance{ static_cast<float>
					(std::pow(pos.x - monster_pos.x, 2)
					+ std::pow(pos.y - monster_pos.y, 2)) };
				if (distance < min_distance) {
					min_distance = distance;
					player = _playerList[i];
				}
			}

			// TODO?: 이거 할거면 하고
			// 가장 가까운 플레이어가 몬스터의 시야에 들어왔는지 체크

			// 몬스터가 플레이어를 쫒아가도록 방향 조정
			auto dir{ player->GetPos() - object->GetPos() };
			dir.Normalize();
			object->SetDir(dir);


			// 일단 몬스터만 움직이게.
			object->Move(delta);
		}
	}
}

void Room::ClearObjects()
{
	_writerObjects.clear();
	_readerObjects.clear();
}

void Room::AddObject(std::shared_ptr<Object> object)
{
	_writerObjects[_idCount++] = object;
}

void Room::InsertPlayers(const int idx, std::shared_ptr<Player>& player)
{
	_playerList[idx] = player;
	_writerObjects[_idCount++] = player;
}

void Room::SendObjectsToClient()
{
	if (RoomType::None == _roomType ||
		RoomStatus::Running != _roomStatus) {
		return;
	}


	// update와 동시에 이루어지지 않으므로 thread-safe
	// 업데이트 이후에 이루어진다.
	const auto& client_ids{ GET_SINGLE(IOCP)->GetClients(_roomId) };
	for (auto& [id, object] : _readerObjects) {
		if (ObjectType::None == object->GetObjectType()) {
			continue;
		}
		
		auto pos{ object->GetPos() };
		auto dir{ object->GetDir() };
		packet::SCMoveObject move_packet{ id, object->GetObjectType(), 
			pos.x, pos.y, dir.x, dir.y};

		for (int i = 0; i < 3; ++i) {
			if (i == id) {
				continue;
			}
			GET_SINGLE(IOCP)->DoSend(client_ids[i], &move_packet);
		}
	}
}


void Room::SyncObjects()
{

	_readerObjects.clear();
	
	// thread safe
	for (auto& obj : _writerObjects) {
		_readerObjects.insert(obj);
	}
}

void Room::Clear()
{
}

