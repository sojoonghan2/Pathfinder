#pragma once


class Room
{
public:
	void InsertPlayer(const int idx, const int player_id);


	// �ӽ÷� �ۺ�
	// TODO: ���߿� private�� ����
public:
	std::array<int, 3> _playerIdList{};

};