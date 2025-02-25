#pragma once
class SocketIO
{
public:
	void Init();
	void Start();

	~SocketIO();

private:
	void Worker();
	// 반환값으로 받아온 버퍼의 길이를 가져옴
	int	 DoRecv();
	void DoSend();

	void ProcessPacket();


private:
	SOCKET serverSocket{ INVALID_SOCKET };
	std::thread	recvThread{};
	std::array<char, BUFFER_SIZE> recvBuffer{};
};

