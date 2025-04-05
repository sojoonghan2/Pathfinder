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
	// ��ȯ������ �޾ƿ� ������ ���̸� ������
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
