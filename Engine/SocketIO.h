#pragma once
#include "NetworkTimer.h"

struct Player
{
	float x{};
	float y{};
};

class SocketIO
{
private:							
	SocketIO() {}

public:								
	static SocketIO* GetInstance()
	{
		static SocketIO instance;	
		return &instance;		
	}

	~SocketIO();

public:

	using BufferType = std::array<char, BUFFER_SIZE>;


	void Init();
	void Update();
	int GetNextId();

	template <class Packet, class ...Args>
	void DoSend(Args ...args);

	
private:
	void Worker();
	// 반환값으로 받아온 버퍼의 길이를 가져옴
	int	 DoRecv();


	void ProcessPacket();


private:
	SOCKET serverSocket{ INVALID_SOCKET };
	std::thread	recvThread{};
	std::queue<BufferType> bufferQueue;
	NetworkTimer sendTimer;
	std::vector<int> idList;
	int idCount{ 0 };

	// temp
public:
	std::unordered_map<int, Player> players{};
	int myId{-1};

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
