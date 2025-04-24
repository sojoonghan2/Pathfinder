#include "pch.h"
#include "Room.h"

void Room::Update(const float delta_time)
{
	// Ȱ��ȭ �Ǿ� �ִ� �游 update�Ѵ�.
	if (RoomType::None == _roomType ||
		RoomStatus::Running != _roomStatus) {
		return;
	}

	for (auto monster : _monsterPtrList) {
		// ���⼭ ������ ������ ��������.

		Player* player = nullptr;
		float min_distance{ std::numeric_limits<float>::max() };
		auto monster_pos{ monster->GetPos() };
		

		// ���� ����� �÷��̾� ã��
		for (int i = 0; i < 3; i++) {
			auto pos{ _playerPtrList[i]->GetPos() };
			auto distance{ static_cast<float>
				(std::pow(pos.x - monster_pos.x, 2)
				+ std::pow(pos.y - monster_pos.y, 2)) };
			if (distance < min_distance) {
				min_distance = distance;
				player = _playerPtrList[i];
			}
		}

		// TODO?: �̰� �ҰŸ� �ϰ�
		// ���� ����� �÷��̾ ������ �þ߿� ���Դ��� üũ

		// ���Ͱ� �÷��̾ �i�ư����� ���� ����
		auto dir{ player->GetPos() - monster->GetPos() };
		dir.Normalize();
		monster->SetDir(dir);
		monster->Update(delta_time);
	}
}



void Room::SetPlayerPtrList(const int id, Player* player_ptr)
{
	_playerPtrList[id] = player_ptr;
}



