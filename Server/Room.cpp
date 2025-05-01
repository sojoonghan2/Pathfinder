#include "pch.h"
#include "Room.h"
#include "IOCP.h"

void Room::Update(const float delta)
{
	// Ȱ��ȭ �Ǿ� �ִ� �游 update�Ѵ�.
	if (RoomType::None == _roomType ||
		RoomStatus::Running != _roomStatus) {
		return;
	}

	// ��ȸ �߰��� ��ü�� ������ ���� ����
	SyncObjects();

	// TODO: �浹 Ȯ��
	// readerObjects �� �浹 üũ
	for (auto iter{ _readerObjects.begin() }; iter != _readerObjects.end(); ++iter) {
		auto iter2{ iter };
		++iter2;
		for (; iter2 != _readerObjects.end(); ++iter2) {
			if (iter->second->CheckCollision(iter2->second)) {
				// Ȯ�� �Ϸ�
			}
		}
	};

	for (auto& [id, object] : _readerObjects) {

		// ����
		if (ObjectType::Monster == object->GetObjectType()) {

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


			// �ϴ� ���͸� �����̰�.
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


	// update�� ���ÿ� �̷������ �����Ƿ� thread-safe
	// ������Ʈ ���Ŀ� �̷������.
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

