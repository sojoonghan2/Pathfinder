#pragma once

class Room
{
public:
	void SetPlayerIdList(const int room_id);
	std::array<int, 3> GetPlayerIdList() const;

	// �ӽ÷� �ۺ�
	// TODO: ���߿� private�� ����
public:
	std::array<int, 3> _playerIdList{};

};