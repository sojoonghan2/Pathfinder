#pragma once


class RoomInfo
{
public:
	bool GetRunning() const;
	bool TrySetRunning(const bool running);


private:
	std::array<int, 3>	_clientIdList{};
	std::atomic_bool	_isRunning{ false };
};

