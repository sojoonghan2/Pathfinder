#include "pch.h"
#include "SocketIO.h"

// temp
extern std::queue<std::array<char, BUFFER_SIZE>> packetQueue;


void SocketIO::Init()
{
	// 윈도우 초기화
	WSADATA wsa;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (ret != 0) {
		exit(-1);
	}

	// 소켓 생성
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		util::DisplayQuitError();
	}

	std::println("SocketIO Init completed.");
}

void SocketIO::Start()
{
	// ADDR 설정
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT_NUMBER);

	// inet_pton 사용하여 IP 주소 변환
	int ret = inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr);
	if (1 != ret) {
		util::DisplayQuitError();
	}

	// connect
	ret = connect(serverSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (SOCKET_ERROR == ret) {
		util::DisplayQuitError();
	}
  
	std::println("Connected to server successfully.");

	// 스레드 생성
	recvThread = std::thread{ [this]() { Worker(); } };
	
	// 로그인 알림
	DoSend<packet::CSLogin>();
}

void SocketIO::Worker()
{
	while (true) {

		int ret = DoRecv();
		if (0 == ret || SOCKET_ERROR == ret) {
			return;
		}
		ProcessPacket();

	}
	std::println("Bye Worker");
}

int SocketIO::DoRecv()
{
	// 기존 버퍼 내용 초기화
	recvBuffer.fill(0);

	// 고정 길이 RECV
	int recv_len = recv(
		serverSocket,
		recvBuffer.data(),
		sizeof(packet::Header),
		MSG_WAITALL);

	// 예외처리
	if (0 == recv_len) {
		std::println("Disconnected from server.");
		return recv_len;
	}
	else if (SOCKET_ERROR == recv_len) {
		util::DisplayQuitError();
	}

	// 정상적으로 받음
	else {
		packet::Header* p_header{ reinterpret_cast<packet::Header*>(recvBuffer.data()) };
		int remain_size = p_header->size - sizeof(packet::Header);

		if (remain_size > 0) {

			// 가변 길이 recv
			recv_len = recv(
				serverSocket,
				recvBuffer.data() + sizeof(packet::Header),
				remain_size,
				MSG_WAITALL);

			// 예외처리
			if (0 == recv_len) {
				std::println("Disconnected from server.");
				return recv_len;
			}
			else if (SOCKET_ERROR == recv_len) {
				util::DisplayQuitError();
			}
		}

		std::println("Successfully Recved.");
	}

	return recv_len;
}

void SocketIO::ProcessPacket()       
{
	
	packetQueue.push(recvBuffer);

}

SocketIO::~SocketIO()
{
	if (recvThread.joinable()) {
		recvThread.join();  // 스레드 종료 대기
	}
	closesocket(serverSocket);
	WSACleanup();
}
