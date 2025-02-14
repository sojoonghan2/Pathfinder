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
	WSAOVERLAPPED	over;				// overlapped IO 구조체
	SOCKET			clientSocket{ INVALID_SOCKET }; // 클라이언트 소켓
	WSABUF			wsabuf{};					// 작업 버퍼
	char			dataBuffer[BUF_SIZE]{};	// 데이터 버퍼
	IOOperation		operation{};				// 동작 종류

	// recv 전용 생성자. 
	OverlappedEx() :
		operation		{ IOOperation::RECV }
	{
		wsabuf.len = BUF_SIZE;
		wsabuf.buf = dataBuffer;
		ZeroMemory(&over, sizeof(over));
	}
};

struct Session {

	// RECV, SEND에 사용할 Overlapped 변수
	OverlappedEx	overEx;
	SOCKET			clientSocket{INVALID_SOCKET};

	Session()
	{
		ZeroMemory(&overEx, sizeof(overEx));
	}
};