#pragma once
class SocketIO
{
public:
	void Init();
	void Start();

	~SocketIO();

	// temp
public:
	void Worker();
	// ��ȯ������ �޾ƿ� ������ ���̸� ������
	int	 DoRecv();


	template <class Packet, class ...Args>
	void DoSend(Args ...args);

	void ProcessPacket();


private:
	SOCKET serverSocket{ INVALID_SOCKET };
	std::thread	recvThread{};
	std::array<char, BUFFER_SIZE> recvBuffer{};
};


template <class Packet, class ...Args>
void SocketIO::DoSend(Args ...args)
{
	// ��Ŷ ����
	Packet packet{ std::forward<Args>(args)... };

	// ���ۿ� ��Ŷ ���� ���
	std::array<char, BUFFER_SIZE> buffer{};
	memcpy(buffer.data(), &packet, sizeof(packet));

	// ������
	int ret = send(serverSocket, buffer.data(), sizeof(packet), 0);
	if (SOCKET_ERROR == ret) {
		util::DisplayError();
	}
}

