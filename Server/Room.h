#pragma once


class Room
{
public:
	bool GetRunning() const;
	bool TrySetRunning(const bool running);


	// �ӽ÷� �ۺ�
// TODO: ���߿� private�� ����
public:
	std::array<int, 3> _clientIdList{};

private:
	std::atomic_bool _isRunning{ false };
};