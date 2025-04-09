#pragma once


class Room
{
public:
	bool GetRunning() const;
	bool TrySetRunning(const bool running);


	// 임시로 퍼블릭
// TODO: 나중에 private로 변경
public:
	std::array<int, 3> _clientIdList{};

private:
	std::atomic_bool _isRunning{ false };
};