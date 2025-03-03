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
	

	template <class _Packet, class ..._Args>
	void DoSend(_Args ...args);

	void ProcessPacket();


private:
	SOCKET serverSocket{ INVALID_SOCKET };
	std::thread	recvThread{};
	std::array<char, BUFFER_SIZE> recvBuffer{};
};


template <class _Packet, class ..._Args>
void SocketIO::DoSend(_Args ...args)
{
	// ��Ŷ ����
	_Packet packet{ std::forward<_Args>(args)... };

	// ���ۿ� ��Ŷ ���� ���
	std::array<char, BUFFER_SIZE> buffer{};
	memcpy(buffer.data(), &packet, sizeof(packet));

	// ������
	int ret = send(serverSocket, buffer.data(), sizeof(packet), 0);
	if (SOCKET_ERROR == ret) {
		util::DisplayError();
	}
}

