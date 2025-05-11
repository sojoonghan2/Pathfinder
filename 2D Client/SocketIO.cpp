#include "pch.h"
#include "SocketIO.h"

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
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == INVALID_SOCKET) {
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

	std::string server_ip{};
	std::cin >> server_ip;

	// inet_pton 사용하여 IP 주소 변환
	int ret = inet_pton(AF_INET, server_ip.c_str(), &serveraddr.sin_addr);
	if (1 != ret) {
		util::DisplayQuitError();
	}

	// connect
	ret = connect(_serverSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (SOCKET_ERROR == ret) {
		util::DisplayQuitError();
	}
  
	std::println("Connected to server successfully.");

	// 스레드 생성
	_recvThread = std::thread{ [this]() { Worker(); } };
	
	// 로그인 알림
	DoSend<packet::CSLogin>();
	DoSend<packet::CSMatchmaking>();
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
		_serverSocket,
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
				_serverSocket,
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
	}

	return recv_len;
}

void SocketIO::ProcessPacket()       
{
	
	packetQueue.push(recvBuffer);

}

SocketIO::~SocketIO()
{
	if (_recvThread.joinable()) {
		_recvThread.join();  // 스레드 종료 대기
	}
	closesocket(_serverSocket);
	WSACleanup();
}