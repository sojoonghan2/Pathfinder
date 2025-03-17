#pragma once

constexpr int ROOM_COUNT = 300;
constexpr int PLAYER_COUNT = 1000;

enum class IOOperation
{
	NONE,
	ACCEPT,
	RECV,
	SEND
};

// temp
struct Room
{
	std::array<int, 3> playerIdList{};
};

struct ClientIdInfo
{
	int playerid{ -1 };
	int roomid{ -1 };
};

struct Player
{
	Vec2f pos{};
	int clientId{ -1 };
	float& x = pos.x;
	float& y = pos.y;

};

struct OverlappedEx
{
	WSAOVERLAPPED	over;					// overlapped IO 구조체
	SOCKET			clientSocket{ INVALID_SOCKET }; // 클라이언트 소켓
	WSABUF			wsabuf{};					// 작업 버퍼
	char			dataBuffer[BUFFER_SIZE]{};		// 데이터 버퍼
	IOOperation		operation{};				// 동작 종류

	// recv 전용 생성자. 
	OverlappedEx() :
		operation		{ IOOperation::RECV }
	{
		wsabuf.len = BUFFER_SIZE;
		wsabuf.buf = dataBuffer;
		ZeroMemory(&over, sizeof(over));
	}

	// send 전용 생성자
	OverlappedEx(unsigned char* packet)
	{
		wsabuf.len = packet[0];
		wsabuf.buf = dataBuffer;
		ZeroMemory(&over, sizeof(over));
		operation = IOOperation::SEND;
		memcpy(dataBuffer, packet, packet[0]);
	}
};

struct ClientInfo
{
	OverlappedEx	overEx;							// RECV에 사용할 Overlapped 변수
	SOCKET			clientSocket{ INVALID_SOCKET };	
	int				currentDataSize{};					// 패킷 재조립을 위한 남은 데이터 수 
	ClientIdInfo	clientIdInfo{};

	ClientInfo()
	{
		ZeroMemory(&overEx, sizeof(overEx));
	}
};


// 싱글턴
#define DECLARE_SINGLE(type)		\
private:							\
	type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\

#define GET_SINGLE(type)	type::GetInstance()