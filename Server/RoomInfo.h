#pragma once

class RoomInfo
{
public:

	// cas
	bool CASRunning(bool old_value, const bool new_value);
	bool CASLoadingCount(int old_value, const int new_value);

	
	bool GetRunning() const;
	void InsertClient(const int idx, const int client_id);
	std::array<int, 3> GetClientIdList() const;

	int GetLoadingCount() const { return _loadingCount.load(); }
	void IncreaseLoadingCount() { ++_loadingCount; }

	void SetLoadingCount(const int count) { _loadingCount.store(count); }

		
private:
	std::array<int, 3>	_clientIdList{};
	std::atomic_bool	_isRunning{ false };
	std::atomic_int		_loadingCount{ -1 };
};

