#include "pch.h"
#include "SocketIO.h"

extern std::queue<std::array<char, BUFFER_SIZE>> packetQueue;

void SocketIO::Init()
{
	// ������ �ʱ�ȭ
	WSADATA wsa;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (ret != 0) {
		exit(-1);
	}

	// ���� ����
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == INVALID_SOCKET) {
		util::DisplayQuitError();
	}

	std::println("SocketIO Init completed.");
}

void SocketIO::Start()
{
	// ADDR ����
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT_NUMBER);

	std::string server_ip{};
	std::cin >> server_ip;

	// inet_pton ����Ͽ� IP �ּ� ��ȯ
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

	// ������ ����
	_recvThread = std::thread{ [this]() { Worker(); } };
	
	// �α��� �˸�
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
	// ���� ���� ���� �ʱ�ȭ
	recvBuffer.fill(0);

	// ���� ���� RECV
	int recv_len = recv(
		_serverSocket,
		recvBuffer.data(),
		sizeof(packet::Header),
		MSG_WAITALL);

	// ����ó��
	if (0 == recv_len) {
		std::println("Disconnected from server.");
		return recv_len;
	}
	else if (SOCKET_ERROR == recv_len) {
		util::DisplayQuitError();
	}

	// ���������� ����
	else {
		packet::Header* p_header{ reinterpret_cast<packet::Header*>(recvBuffer.data()) };
		int remain_size = p_header->size - sizeof(packet::Header);

		if (remain_size > 0) {

			// ���� ���� recv
			recv_len = recv(
				_serverSocket,
				recvBuffer.data() + sizeof(packet::Header),
				remain_size,
				MSG_WAITALL);

			// ����ó��
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
		_recvThread.join();  // ������ ���� ���
	}
	closesocket(_serverSocket);
	WSACleanup();
}