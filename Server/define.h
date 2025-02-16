#pragma once

constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

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
	char			dataBuffer[BUF_SIZE]{};		// ������ ����
	IOOperation		operation{};				// ���� ����

	// recv ���� ������. 
	OverlappedEx() :
		operation		{ IOOperation::RECV }
	{
		wsabuf.len = BUF_SIZE;
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