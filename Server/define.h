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
	WSAOVERLAPPED	overlapped;				// overlapped IO 구조체
	SOCKET			client_socket;			// 클라이언트 소켓
	WSABUF			wsabuf;					// 작업 버퍼
	char			data_buffer[BUF_SIZE];	// 데이터 버퍼
	IOOperation		operation;				// 동작 종류

	// recv 전용 생성자. 
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

	// RECV, SEND에 사용할 Overlapped 변수
	OverlappedEx	overlapped_ex;
	SOCKET			socket;

	Session() :
		socket	{ INVALID_SOCKET }
	{
		ZeroMemory(&overlapped_ex, sizeof(overlapped_ex));
	}
};