#pragma once

#include "Player.h"

class IOCP
{
public:
	bool Init();
	bool Start();


	~IOCP();

private:

	void Worker();
	void DoRecv(Session& session) const;
	void DoSend(Session& session, void* packet);
	bool ProcessPacket(int key, char* p);
	void DoBroadcast(void* packet);


private:
	HANDLE		IOCPHandle{ INVALID_HANDLE_VALUE };
	int			sessionCnt{};
	SOCKET		listenSocket{ INVALID_SOCKET };
	SOCKET		acceptSocket{ INVALID_SOCKET };

	std::array<Session, 10> sessionList{};
	std::vector<std::thread> workers{};


	// temp
public:
	std::array<Vec2f, 3> players;

};

