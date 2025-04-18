#pragma once


class RoomInfo
{
public:
	bool GetRunning() const;
	bool CompareSetRunning(bool old_value, const bool new_value);
	void InsertClient(const int idx, const int client_id);
	std::array<int, 3> GetClientIdList() const;

	bool TrySetCount(int old_value, const int new_value);
	int GetLoadCount() const { return _loadCount.load(); }
		
private:
	std::array<int, 3>	_clientIdList{};
	std::atomic_bool	_isRunning{ false };
	std::atomic_int		_loadCount{ -1 };
};

