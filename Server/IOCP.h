#pragma once

#include "Player.h"
#include "RoomInfo.h"

class IOCP
{
	DECLARE_SINGLE(IOCP)

public:
	bool Init();
	bool Start();


public:
	void DoSend(int client_id, void* packet);

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
	HANDLE		_IOCPHandle{ INVALID_HANDLE_VALUE };
	int			_sessionCnt{0};
	SOCKET		_listenSocket{ INVALID_SOCKET };
	SOCKET		_acceptSocket{ INVALID_SOCKET };


	std::vector<std::thread> _workers{};

	// TODO:
	// �ϴ�	concurrent_unordered_map�� ���, ���߿��� �׳� unordered_map �ٲٱ�
	concurrency::concurrent_unordered_map<int, ClientInfo> _clientInfoHash;
	
	std::array<RoomInfo, MAX_ROOM> _roomClientList{};

	concurrency::concurrent_queue<int> _matchmakingQueue{};

};

