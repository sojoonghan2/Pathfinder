#pragma once

class IOCP
{

public:
	bool Init();

	~IOCP();


private:
	void Worker();
	void TimerWorker();
	void DoRecv(ClientInfo& client_info) const;
	void DoSend(ClientInfo& client_info, void* packet);
	bool ProcessPacket(int key, char* p);
	void Disconnect(const int client_id);
	void LoginWorker();

private:
	HANDLE		IOCPHandle{ INVALID_HANDLE_VALUE };
	int			sessionCnt{ 0 };

	std::array<ClientInfo, MAX_PLAYER> players;

	std::vector<std::thread> workers{};
};

