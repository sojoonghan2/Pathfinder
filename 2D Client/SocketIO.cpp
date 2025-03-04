#include "pch.h"
#include "SocketIO.h"

// temp
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
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
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

	// inet_pton ����Ͽ� IP �ּ� ��ȯ
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

	// ������ ����
	recvThread = std::thread{ [this]() { Worker(); } };
	
	// �α��� �˸�
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
	// ���� ���� ���� �ʱ�ȭ
	recvBuffer.fill(0);

	// ���� ���� RECV
	int recv_len = recv(
		serverSocket,
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
				serverSocket,
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
		recvThread.join();  // ������ ���� ���
	}
	closesocket(serverSocket);
	WSACleanup();
}
