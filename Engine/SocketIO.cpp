#include "pch.h"
#include "SocketIO.h"

#include "Input.h"
#include "Timer.h"
#include "MessageManager.h"

// 플레이어 이동속도, 단위 m/s
constexpr float PLAYER_SPEED_MPS{ 5.f };
constexpr float PLAYER_SPEED_MPMS{ PLAYER_SPEED_MPS / 1000.f };

// 단위 m
constexpr float MAP_SIZE_M{ 50.f };
constexpr float PLAYER_SIZE_M{ 0.5f };

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

	// ADDR 설정
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT_NUMBER);

	// inet_pton 사용하여 IP 주소 변환
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

	// 스레드 생성
	recvThread = std::thread{ [this]() { Worker(); } };

	// 로그인 알림
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
	// 기존 버퍼 내용 초기화
	BufferType buffer{};

	// 고정 길이 RECV
	int recv_len = recv(
		serverSocket,
		buffer.data(),
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
		packet::Header* p_header{ reinterpret_cast<packet::Header*>(buffer.data()) };
		int remain_size = p_header->size - sizeof(packet::Header);

		if (remain_size > 0) {
			// 가변 길이 recv
			recv_len = recv(
				serverSocket,
				buffer.data() + sizeof(packet::Header),
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
		recvThread.join();  // 스레드 종료 대기
	}
	closesocket(serverSocket);
	WSACleanup();
}