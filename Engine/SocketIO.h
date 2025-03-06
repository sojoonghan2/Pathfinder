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
	// 반환값으로 받아온 버퍼의 길이를 가져옴
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
	// 패킷 생성
	Packet packet{ std::forward<Args>(args)... };

	// 버퍼에 패킷 내용 담기
	std::array<char, BUFFER_SIZE> buffer{};
	memcpy(buffer.data(), &packet, sizeof(packet));

	// 보내기
	int ret = send(serverSocket, buffer.data(), sizeof(packet), 0);
	if (SOCKET_ERROR == ret) {
		util::DisplayError();
	}
}

