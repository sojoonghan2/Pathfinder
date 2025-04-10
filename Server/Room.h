#pragma once

class Room
{
public:
	void SetPlayerIdList(const int room_id);
	std::array<int, 3> GetPlayerIdList() const;

	// 임시로 퍼블릭
	// TODO: 나중에 private로 변경
public:
	std::array<int, 3> _playerIdList{};

};