#include "pch.h"
#include "Room.h"
#include "IOCP.h"
#include "Game.h"
#include "Bullet.h"
#include "Monster.h"

void Room::Update(const float delta)
{
	// Ȱ��ȭ �Ǿ� �ִ� �游 update�Ѵ�.
	if (RoomType::NONE == _roomType ||
		RoomStatus::Running != _roomStatus) {
		return;
	}

	// ���� ����� ���� ���� ��ȸ
	// TODO: ���� �񱳸� ���־�� �ҵ�.
	std::unordered_map<int, std::shared_ptr<Object>> objects;
	std::unordered_set<int> deleted_objects;
	for (auto& [id, object] : _objects) {
		if (object == nullptr) {
			continue;
		}
		objects[id] = object;
		
	}

	for (auto& [id, object] : objects) {

		// ����
		if (ObjectType::MONSTER == object->GetObjectType()) {

			std::shared_ptr<Player> player = nullptr;
			float min_distance{ std::numeric_limits<float>::max() };
			auto monster_pos{ object->GetPos() };

			// ���� ����� �÷��̾� ã��
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

			// TODO?: �̰� �ҰŸ� �ϰ�
			// ���� ����� �÷��̾ ������ �þ߿� ���Դ��� üũ

			// ���Ͱ� �÷��̾ �i�ư����� ���� ����
			auto dir{ player->GetPos() - object->GetPos() };
			dir.Normalize();
			object->SetDir(dir);
		}

		// ��ü ������Ʈ
		if (ObjectType::PLAYER != object->GetObjectType()) {
			object->Update(delta);

			// �ӽ�
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

			// �浹 ó�� �� �̹� ������ ��ü�� ��ŵ
			if (deleted_objects.contains(iter1->first) || deleted_objects.contains(iter2->first)) {
				continue;
			}

			// Ȥ�� �� ó��
			if (iter2->second == nullptr || iter1->second == nullptr) {
				continue;
			}

			// �浹 üũ �˻� 
			if (iter1->second->CheckCollision(iter2->second)) {
				auto iter_a{ iter1 };
				auto iter_b{ iter2 };
				auto type_a{ iter1->second->GetObjectType() };
				auto type_b{ iter2->second->GetObjectType() };

				if (type_a > type_b) {
					std::swap(iter_a, iter_b);
					std::swap(type_a, type_b);
				}

				// ����!!!! first�� �׻� ���� ���� ������ �Ѵ�.
				// �Ѿ˰� ������ �浹
				if (type_a == ObjectType::MONSTER && type_b == ObjectType::BULLET) {
					auto monster{ std::static_pointer_cast<Monster>(iter_a->second) };
					auto bullet{ std::static_pointer_cast<Bullet>(iter_b->second) };

					monster->DecreaseHp(PLAYER_BULLET_DAMAGE);
				
					// ��� �÷��̾�� ü�� ���� ��Ŷ�� ����
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

	// �������� ���ŵ� ��ü�� �����
	// ������ ���⼭�� �̷������, ���� ��ü�� �������� �����Ƿ� thread-safe
	// ������ id�� 
	for (auto& id : deleted_objects) {
		auto iter{ _objects.find(id) };
		if (iter != _objects.end()) {
			_objects.unsafe_erase(iter);
		}
	}

	// ������ id�� �÷��̾�� ����
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


	// update�� ���ÿ� �̷������ �����Ƿ� thread-safe
	// ������Ʈ ���Ŀ� �̷������.
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

