#pragma once
#include "NetworkTimer.h"

//struct Player
//{
//	float x{};
//	float y{};
//};

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


	RoomType GetRoomType() const { return _roomType; }

	template <class Packet, class ...Args>
	void DoSend(Args ...args);

	void Continue();

	
private:
	void Worker();
	// 반환값으로 받아온 버퍼의 길이를 가져옴
	int	 DoRecv();


	void ProcessPacket();

private:

	SOCKET		_serverSocket{ INVALID_SOCKET };
	std::thread	_recvThread{};
	std::queue<BufferType> _bufferQueue;
	NetworkTimer _sendTimer;
	RoomType	_roomType{ RoomType::NONE };
	int			_myId{-1};

	bool		_stop{ true };
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
	int ret = send(_serverSocket, buffer.data(), sizeof(packet), 0);
	if (SOCKET_ERROR == ret) {
		util::DisplayError();
	}
}
