#pragma once

constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

enum class IOOperation
{
	ACCEPT,
	RECV,
	SEND
};


struct OverlappedEx
{
	WSAOVERLAPPED	overlapped;				// overlapped IO ����ü
	SOCKET			client_socket;			// Ŭ���̾�Ʈ ����
	WSABUF			wsabuf;					// �۾� ����
	char			data_buffer[BUF_SIZE];	// ������ ����
	IOOperation		operation;				// ���� ����

	// recv ���� ������. 
	OverlappedEx() :
		client_socket	{ INVALID_SOCKET },
		data_buffer		{},
		operation		{ IOOperation::RECV }
	{
		wsabuf.len = BUF_SIZE;
		wsabuf.buf = data_buffer;
		ZeroMemory(&overlapped, sizeof(overlapped));
	}
};

struct Session {

	// RECV, SEND�� ����� Overlapped ����
	OverlappedEx	overlapped_ex;
	SOCKET			socket;

	Session() :
		socket	{ INVALID_SOCKET }
	{
		ZeroMemory(&overlapped_ex, sizeof(overlapped_ex));
	}
};