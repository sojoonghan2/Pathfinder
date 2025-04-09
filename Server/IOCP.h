#pragma once

#include "Player.h"

class IOCP
{
	DECLARE_SINGLE(IOCP)

public:
	bool Init();
	bool Start();


public:
	void DoSend(int client_id, void* packet);

	void SetClientIdInfo(int client_id, int player_id, int room_id);
	IOState GetClientIOState(int client_id);

public:
	~IOCP();

private:

	void Worker();
	void TimerWorker();
	void DoRecv(ClientInfo& client_info) const;
	void ProcessPacket(int key, char* p);
	void DoBroadcast(void* packet);
	void DoBroadcast(int kcey, void* packet);
	void Disconnect(int client_id);
	void DoSend(ClientInfo& client_info, void* packet);

private:
	HANDLE		IOCPHandle{ INVALID_HANDLE_VALUE };
	int			sessionCnt{0};
	SOCKET		listenSocket{ INVALID_SOCKET };
	SOCKET		acceptSocket{ INVALID_SOCKET };


	std::vector<std::thread> workers{};
	concurrency::concurrent_unordered_map<int, ClientInfo> clientInfoHash;
	concurrency::concurrent_queue<int> _matchmakingQueue{};

};

