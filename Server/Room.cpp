#include "pch.h"
#include "Room.h"
#include "IOCP.h"
#include "Game.h"
#include "Bullet.h"
#include "Monster.h"

void Room::Update(const float delta)
{
	// 활성화 되어 있는 방만 update한다.
	if (RoomType::NONE == _roomType ||
		RoomStatus::Running != _roomStatus) {
		return;
	}

	// 성능 향상을 위한 복사 순회
	// TODO: 성능 비교를 해주어야 할듯.
	std::unordered_map<int, std::shared_ptr<Object>> objects;
	std::unordered_set<int> deleted_objects;
	for (auto& [id, object] : _objects) {
		if (object == nullptr) {
			continue;
		}
		objects[id] = object;
		
	}

	for (auto& [id, object] : objects) {

		// 몬스터
		if (ObjectType::MONSTER == object->GetObjectType()) {

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
		}

		// 객체 업데이트
		if (ObjectType::PLAYER != object->GetObjectType()) {
			object->Update(delta);

			// 임시
			if (ObjectType::BULLET == object->GetObjectType()) {
				if (object->GetPos().x < -24.f) {
					deleted_objects.insert(id);
				}
				else if (object->GetPos().x > 24.f) {
					deleted_objects.insert(id);
				}
				else if (object->GetPos().y < -24.f) {
					deleted_objects.insert(id);
				}
				else if (object->GetPos().y > 24.f) {
					deleted_objects.insert(id);
				}
			}
		}
	}



	for (auto iter1{ objects.begin() }; iter1 != objects.end(); ++iter1) {
		auto iter2{ iter1 };
		++iter2;
		for (; iter2 != objects.end(); ++iter2) {

			// 충돌 처리 중 이미 지워진 객체면 스킵
			if (deleted_objects.contains(iter1->first) || deleted_objects.contains(iter2->first)) {
				continue;
			}

			// 혹기 모를 처리
			if (iter2->second == nullptr || iter1->second == nullptr) {
				continue;
			}

			// 충돌 체크 검사 
			if (iter1->second->CheckCollision(iter2->second)) {
				auto iter_a{ iter1 };
				auto iter_b{ iter2 };
				auto type_a{ iter1->second->GetObjectType() };
				auto type_b{ iter2->second->GetObjectType() };

				if (type_a > type_b) {
					std::swap(iter_a, iter_b);
					std::swap(type_a, type_b);
				}

				// 주의!!!! first는 항상 작은 값이 들어가도록 한다.
				// 총알과 몬스터의 충돌
				if (type_a == ObjectType::MONSTER && type_b == ObjectType::BULLET) {
					auto monster{ std::static_pointer_cast<Monster>(iter_a->second) };
					auto bullet{ std::static_pointer_cast<Bullet>(iter_b->second) };

					monster->DecreaseHp(PLAYER_BULLET_DAMAGE);
				
					// 모든 플레이어에게 체력 감소 패킷을 전달
					const auto& client_ids{ GET_SINGLE(IOCP)->GetClients(_roomId) };
					packet::SCSetObjectHp monster_hp_packet{
						iter_a->first, monster->GetHp(), bullet->GetPlayerId()
					};
					for (auto client_id : client_ids) {
						GET_SINGLE(IOCP)->DoSend(client_id, &monster_hp_packet);
					}
					if (monster->IsDead()) {
						deleted_objects.insert(iter_a->first);
					}

					deleted_objects.insert(iter_b->first);
				}
			}
		}
	}

	// 논리적으로 제거된 객체들 지우기
	// 삭제는 여기서만 이루어지고, 지운 객체는 참조하지 않으므로 thread-safe
	// 지워진 id를 
	for (auto& id : deleted_objects) {
		auto iter{ _objects.find(id) };
		if (iter != _objects.end()) {
			_objects.unsafe_erase(iter);
		}
	}

	// 지워진 id를 플레이어에게 전송
	const auto& client_ids{ GET_SINGLE(IOCP)->GetClients(_roomId) };
	for (auto& object_id : deleted_objects) {
		packet::SCDeleteObject delete_packet{ object_id };
		for (auto client_id : client_ids) {
			GET_SINGLE(IOCP)->DoSend(client_id, &delete_packet);
		}
	}
}

void Room::ClearObjects()
{
	_objects.clear();
	_idCount = 0;
}

void Room::AddObject(std::shared_ptr<Object> object)
{
	if (nullptr == object) {
		return;
	}
	_objects[_idCount++] = object;
}

void Room::InsertPlayers(const int idx, std::shared_ptr<Player>& player)
{
	_playerList[idx] = player;
	_objects[_idCount++] = player;
}

void Room::FireBullet(const int player_id)
{
	auto player{ _playerList[player_id] };


	auto obj{ GET_SINGLE(Game)->GetObjectFromPool(ObjectType::BULLET) };
	auto bullet{ std::static_pointer_cast<Bullet>(obj) };
	bullet->InitBullet(player->GetPos(), player->GetDir(), player_id);
	AddObject(bullet);
}

void Room::SendObjectsToClient()
{
	if (RoomType::NONE == _roomType ||
		RoomStatus::Running != _roomStatus) {
		return;
	}


	// update와 동시에 이루어지지 않으므로 thread-safe
	// 업데이트 이후에 이루어진다.
	const auto& client_ids{ GET_SINGLE(IOCP)->GetClients(_roomId) };
	for (auto& [id, object] : _objects) {
		if (ObjectType::NONE == object->GetObjectType()) {
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
			std::cout << _objects.size() << std::endl;
		}
	}
}


void Room::Clear()
{
}

