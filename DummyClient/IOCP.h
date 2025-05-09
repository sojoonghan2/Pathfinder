#pragma once

class IOCP
{
	using Timepoint = std::chrono::steady_clock::time_point;

public:
	bool Init();
	int GetDelayTime() const;
	int GetPlayerCount() const;

	~IOCP();


private:
	void Worker();
	void DoRecv(Session& client_info) const;
	void DoSend(Session& client_info, void* packet);
	bool ProcessPacket(int key, char* p);
	void Disconnect(const int client_id);
	void LoginWorker();
	void TimerWorker();

private:
	HANDLE		_IOCPHandle{ INVALID_HANDLE_VALUE };

	std::array<Session, MAX_PLAYER> players;
	std::array<Timepoint, MAX_PLAYER> delays;

	std::atomic<int>	delayTime{ 0 };
	std::atomic<int>	currentClient{ 0 };

	std::vector<std::thread> _workers{};
};

