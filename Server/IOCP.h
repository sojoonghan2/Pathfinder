#pragma once

class IOCP
{
public:
	bool InitServer();
	bool StartServer();


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
	std::array<Session, 10> sessionList{};
};

