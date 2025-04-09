#include "pch.h"
#include "SocketIO.h"

#include "Input.h"
#include "Timer.h"
#include "MessageManager.h"

// �÷��̾� �̵��ӵ�, ���� m/s
constexpr float PLAYER_SPEED_MPS{ 5.f };
constexpr float PLAYER_SPEED_MPMS{ PLAYER_SPEED_MPS / 1000.f };

// ���� m
constexpr float MAP_SIZE_M{ 50.f };
constexpr float PLAYER_SIZE_M{ 0.5f };

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

	// ADDR ����
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT_NUMBER);

	// inet_pton ����Ͽ� IP �ּ� ��ȯ
	ret = inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr);
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

void SocketIO::Update()
{
	ProcessPacket();

	if (-1 != myId) {
		if (INPUT->GetButton(KEY_TYPE::UP)) {
			players[myId].y += PLAYER_SPEED_MPS * DELTA_TIME;
			players[myId].y = min(players[myId].y, (MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f);
		}
		if (INPUT->GetButton(KEY_TYPE::DOWN)) {
			players[myId].y -= PLAYER_SPEED_MPS * DELTA_TIME;
			players[myId].y = max(players[myId].y, -(MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f);
		}
		if (INPUT->GetButton(KEY_TYPE::LEFT)) {
			players[myId].x -= PLAYER_SPEED_MPS * DELTA_TIME;
			players[myId].x = min(players[myId].x, (MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f);
		}
		if (INPUT->GetButton(KEY_TYPE::RIGHT)) {
			players[myId].x += PLAYER_SPEED_MPS * DELTA_TIME;
			players[myId].x = max(players[myId].x, -(MAP_SIZE_M - PLAYER_SIZE_M) * 0.5f);
		}

		if (sendTimer.PeekDeltaTime() > 50.f) {
			sendTimer.updateDeltaTime();
			DoSend<packet::CSMovePlayer>(myId, players[myId].x, players[myId].y);
		}
	}
}

void SocketIO::Worker()
{
	while (true) {
		int ret = DoRecv();
		if (0 == ret || SOCKET_ERROR == ret) {
			return;
		}
	}
}

int SocketIO::DoRecv()
{
	// ���� ���� ���� �ʱ�ȭ
	BufferType buffer{};

	// ���� ���� RECV
	int recv_len = recv(
		serverSocket,
		buffer.data(),
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
		packet::Header* p_header{ reinterpret_cast<packet::Header*>(buffer.data()) };
		int remain_size = p_header->size - sizeof(packet::Header);

		if (remain_size > 0) {
			// ���� ���� recv
			recv_len = recv(
				serverSocket,
				buffer.data() + sizeof(packet::Header),
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

	bufferQueue.emplace(buffer);
	return recv_len;
}

void SocketIO::ProcessPacket()
{
	while (not bufferQueue.empty()) {
		auto& buffer = bufferQueue.front();
		packet::Header& header = reinterpret_cast<packet::Header&>(buffer);
		switch (header.type) {
		case packet::Type::SC_LOGIN:
		{
			packet::SCLogin packet = reinterpret_cast<packet::SCLogin&>(buffer);
			std::println("SC_LOGIN, id : {}", packet.playerId);
			myId = packet.playerId;
		}
		break;
		case packet::Type::SC_MOVE_PLAYER:
		{
			packet::SCMovePlayer packet = reinterpret_cast<packet::SCMovePlayer&>(buffer);

			GET_SINGLE(MessageManager)->InsertMessage(packet.playerId, packet.x, packet.y);
			players[packet.playerId].x = packet.x;
			players[packet.playerId].y = packet.y;
		}
		break;
		default:
		{
			std::println("Packet Error.");
		}
		break;
		}
		bufferQueue.pop();
	}
}

int SocketIO::GetNextId()
{
	if (-1 == myId) { return -1; }
	if (nextId == myId) { ++nextId; }
	return nextId++;
}

SocketIO::~SocketIO()
{
	if (recvThread.joinable()) {
		recvThread.join();  // ������ ���� ���
	}
	closesocket(serverSocket);
	WSACleanup();
}