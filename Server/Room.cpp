#include "pch.h"
#include "Room.h"

void Room::Update(const float delta_time)
{
	// Ȱ��ȭ �Ǿ� �ִ� �游 update�Ѵ�.
	if (RoomType::None == _roomType ||
		RoomStatus::Running != _roomStatus) {
		return;
	}

	SyncObjects();
	// �浹 Ȯ��
	



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
			object->Move(delta_time);
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


void Room::SyncObjects()
{

	_readerObjects.clear();
	
	// thread safe
	for (auto& obj : _writerObjects) {
		_readerObjects.insert(obj);
	}
}



