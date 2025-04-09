#pragma once

constexpr int MAX_MONSTER = MAX_ROOM * 10;


struct Vec2f
{
	float x{};
	float y{};

	Vec2f() = default;
	Vec2f(const float x, const float y) :
		x{ x },
		y{ y }
	{}
};

struct ClientIdInfo
{
	int playerId{ -1 };
	int roomId{ -1 };
};

enum class IOOperation
{
	NONE,
	ACCEPT,
	RECV,
	SEND
};

enum class IOState
{
	NONE,
	CONNECT,
	INGAME,
	DISCONNECT
};


struct alignas(64) OverlappedEx
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
	OverlappedEx	overEx;								// RECV�� ����� Overlapped ����
	SOCKET			clientSocket{ INVALID_SOCKET };	
	int				currentDataSize{};					// ��Ŷ �������� ���� ���� ������ �� 
	ClientIdInfo	clientIdInfo{};
	IOState			ioState{ IOState::NONE };

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