#pragma once


class Room
{
public:
	void InsertPlayer(const int idx, const int player_id);


	// 임시로 퍼블릭
	// TODO: 나중에 private로 변경
public:
	std::array<int, 3> _playerIdList{};

};