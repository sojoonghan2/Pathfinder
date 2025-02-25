#pragma once
class SocketIO
{
public:
	void Init();
	void Start();

	~SocketIO();

private:
	void Worker();
	// ��ȯ������ �޾ƿ� ������ ���̸� ������
	int	 DoRecv();
	void DoSend();

	void ProcessPacket();


private:
	SOCKET serverSocket{ INVALID_SOCKET };
	std::thread	recvThread{};
	std::array<char, BUFFER_SIZE> recvBuffer{};
};

