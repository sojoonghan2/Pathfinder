#pragma once
class SocketIO
{
public:
	void Init();
	void Start();

	~SocketIO();

private:
	void Worker();
	int	 DoRecv();


private:
	SOCKET serverSocket{ INVALID_SOCKET };
	std::thread	recvThread{};
	std::array<char, BUFFER_SIZE> recvBuffer{};
};

