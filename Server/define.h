#pragma once

constexpr int ROOM_COUNT = 100;

enum class IOOperation
{
	NONE,
	ACCEPT,
	RECV,
	SEND
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

struct Session
{
	OverlappedEx	overEx;							// RECV�� ����� Overlapped ����
	SOCKET			clientSocket{ INVALID_SOCKET };	
	int				currentDataSize{};					// ��Ŷ �������� ���� ���� ������ �� 

	Session()
	{
		ZeroMemory(&overEx, sizeof(overEx));
	}
};

// �̱���
#define DECLARE_SINGLE(type)		\
private:							\
	type() {}						\
	~type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\

#define GET_SINGLE(type)	type::GetInstance()