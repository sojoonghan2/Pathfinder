#pragma once


class RoomInfo
{
public:
	bool GetRunning() const;
	bool TrySetRunning(const bool running);
	void InsertClient(const int idx, const int client_id);
	std::array<int, 3> GetClientIdList() const
	{
		return _clientIdList;
	}

		
private:
	std::array<int, 3>	_clientIdList{};
	std::atomic_bool	_isRunning{ false };
};

