#pragma once

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

struct Player
{
	Vec2f pos{};
	int clientId{ -1 };
	float& x = pos.x;
	float& y = pos.y;
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


struct OverlappedEx
{
	WSAOVERLAPPED	over;					// overlapped IO ����ü
	SOCKET			clientSocket{ INVALID_SOCKET }; // Ŭ���̾�Ʈ ����
	WSABUF			wsabuf{};					// �۾� ����
	char			dataBuffer[BUFFER_SIZE]{};		// ������ ����
	IOOperation		operation{};				// ���� ����

	// recv ���� ������. 
	OverlappedEx() :
		operation{ IOOperation::RECV }
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

struct Session
{
	OverlappedEx	overEx;							// RECV�� ����� Overlapped ����
	SOCKET			clientSocket{ INVALID_SOCKET };
	int				currentDataSize{};
	IOState			ioState{ IOState::DISCONNECT };

	Session()
	{
		ZeroMemory(&overEx, sizeof(overEx));
	}
};