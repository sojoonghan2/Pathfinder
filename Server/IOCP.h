#pragma once

#include "Player.h"

class IOCP
{
	DECLARE_SINGLE(IOCP)

public:
	bool Init();
	bool Start();


public:
	~IOCP();

private:

	void Worker();
	void TimerWorker();
	void DoRecv(ClientInfo& client_info) const;
	void DoSend(ClientInfo& client_info, void* packet);
	bool ProcessPacket(int key, char* p);
	void DoBroadcast(void* packet);
	void DoBroadcast(int key, void* packet);

private:
	HANDLE		IOCPHandle{ INVALID_HANDLE_VALUE };
	int			sessionCnt{0};
	SOCKET		listenSocket{ INVALID_SOCKET };
	SOCKET		acceptSocket{ INVALID_SOCKET };

	std::vector<std::thread> workers{};
	concurrency::concurrent_unordered_map<int, ClientInfo> clientInfoHash;


};

