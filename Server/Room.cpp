#include "pch.h"
#include "Room.h"

void Room::Update(const float delta_time)
{
	// 활성화 되어 있는 방만 update한다.
	if (RoomType::None == _roomType ||
		RoomStatus::Running != _roomStatus) {
		return;
	}

	for (auto monster : _monsterPtrList) {
		// 여기서 몬스터의 방향을 정해주자.

		Player* player = nullptr;
		float min_distance{ std::numeric_limits<float>::max() };
		auto monster_pos{ monster->GetPos() };
		

		// 가장 가까운 플레이어 찾기
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

		// TODO?: 이거 할거면 하고
		// 가장 가까운 플레이어가 몬스터의 시야에 들어왔는지 체크

		// 몬스터가 플레이어를 쫒아가도록 방향 조정
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



