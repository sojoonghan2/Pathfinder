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
	WSAOVERLAPPED	over;					// overlapped IO ����ü
	SOCKET			clientSocket{ INVALID_SOCKET }; // Ŭ���̾�Ʈ ����
	WSABUF			wsabuf{};					// �۾� ����
	char			dataBuffer[BUFFER_SIZE]{};		// ������ ����
	IOOperation		operation{};				// ���� ����

	// recv ���� ������. 
	OverlappedEx() :
		operation		{ IOOperation::RECV }
	{
		wsabuf.len = BUFFER_SIZE;
		wsabuf.buf = dataBuffer;
		ZeroMemory(&over, sizeof(over));
	}

	// send ���� ������
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
	OverlappedEx	overEx;							// RECV�� ����� Overlapped ����
	SOCKET			clientSocket{ INVALID_SOCKET };	
	int				currentDataSize{};					// ��Ŷ �������� ���� ���� ������ �� 
	ClientIdInfo	clientIdInfo{};

	ClientInfo()
	{
		ZeroMemory(&overEx, sizeof(overEx));
	}
};


// �̱���
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