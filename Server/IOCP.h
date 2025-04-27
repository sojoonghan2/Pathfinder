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
	~IOCP();

private:

	void Worker();
	void TimerWorker();
	void DoRecv(ClientInfo& client_info) const;
	void ProcessPacket(int key, char* p);

	void DoBroadcast(void* packet);
	void DoBroadcast(int kcey, void* packet);
	void Disconnect(int client_id);

	void DoSend(int client_id, void* packet);
	void DoSend(ClientInfo& client_info, void* packet);

private:
	HANDLE		_IOCPHandle{ INVALID_HANDLE_VALUE };
	int			_sessionCnt{0};
	SOCKET		_listenSocket{ INVALID_SOCKET };
	SOCKET		_acceptSocket{ INVALID_SOCKET };


	std::vector<std::thread> _workers{};

	// TODO:
	// 이렇게 하면 delete가 안됨.
	// 일단 기본으로 사용 나중에 atomic_shared_ptr로 바꿔야함
	concurrency::concurrent_unordered_map<int, ClientInfo> _clientInfoHash;
	
	std::array<RoomInfo, MAX_ROOM>		_roomInfoList{};

	concurrency::concurrent_queue<int>	_matchmakingQueue{};
	std::mutex _matchmakingLock;

};

