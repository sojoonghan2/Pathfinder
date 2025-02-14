#pragma once

class IOCP
{
public:
	bool InitServer();
	bool StartServer();


	~IOCP();

private:

	void worker();


private:
	HANDLE		IOCPHandle{ INVALID_HANDLE_VALUE };
	int			sessionCnt{};
	SOCKET		listenSocket{ INVALID_SOCKET };
	std::array<Session, 10> sessionList{};
};

