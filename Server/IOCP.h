#pragma once

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
	void ProcessPacket(int key, char* p);


private:
	HANDLE		IOCPHandle{ INVALID_HANDLE_VALUE };
	int			sessionCnt{};
	SOCKET		listenSocket{ INVALID_SOCKET };
	SOCKET		acceptSocket{ INVALID_SOCKET };

	std::array<Session, 10> sessionList{};
};

