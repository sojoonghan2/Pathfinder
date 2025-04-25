#include "pch.h"
#include "SocketIO.h"

#include "Input.h"
#include "Timer.h"
#include "MessageManager.h"
#include "SceneManager.h"


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
	ret = connect(_serverSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (SOCKET_ERROR == ret) {
		util::DisplayQuitError();
	}

	std::println("Connected to server successfully.");

	// 스레드 생성
	_recvThread = std::thread{ [this]() { Worker(); } };

	// 로그인 알림
	DoSend<packet::CSLogin>();
}

void SocketIO::Update()
{
	ProcessPacket();

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
		_serverSocket,
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
				_serverSocket,
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

	_bufferQueue.emplace(buffer);
	return recv_len;
}

void SocketIO::ProcessPacket()
{
	while (not _bufferQueue.empty()) {
		auto& buffer = _bufferQueue.front();
		packet::Header& header = reinterpret_cast<packet::Header&>(buffer);
		switch (header.type) {
		case packet::Type::SC_LOGIN:
		{

		}
		break;
		case packet::Type::SC_GAME_START:
		{
			// 게임 시작 메시지를 보내자
			// todo:
			// 일단 임시로 ruinsScene에 보냄

			auto msg{ std::make_shared<MsgStartGame>() };
			GET_SINGLE(MessageManager)->PushMessage(ID_RUIN_SCENE, msg);

		}
		break;
		case packet::Type::SC_MATCHMAKING:
		{
			packet::SCMatchmaking packet = reinterpret_cast<packet::SCMatchmaking&>(buffer);
			std::cout << "MATCHMAKING COMPLETED, id : " << packet.clientId << std::endl;
			_myId = packet.clientId;
			_roomType = packet.roomType;

			// todo: 예외처리 필요한가?
			GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");

			// todo:
			// 로딩 신으로 넘기라는 메시지를 보내자
		}
		break;

		case packet::Type::SC_MOVE_PLAYER:
		{
			packet::SCMovePlayer packet = reinterpret_cast<packet::SCMovePlayer&>(buffer);
			if (_idList.end() == std::find(_idList.begin(), _idList.end(), packet.clientId) &&
				packet.clientId != _myId) {
				_idList.push_back(packet.clientId);
			}
			auto msg{ std::make_shared<MsgMove>(packet.x, packet.y, packet.dirX, packet.dirY) };
			GET_SINGLE(MessageManager)->PushMessage(packet.clientId, msg);
		}
		break;

		case packet::Type::SC_MOVE_MONSTER:
		{
			packet::SCMoveMonster packet{ reinterpret_cast<packet::SCMoveMonster&>(buffer) };
			if (_monsterIdList.end() == std::find(_monsterIdList.begin(), _monsterIdList.end(), packet.monsterId)) {
				_monsterIdList.push_back(packet.monsterId);
			}
			auto msg{ std::make_shared<MsgMove>(packet.x, packet.y, packet.dirX, packet.dirY) };
			GET_SINGLE(MessageManager)->PushMessage(packet.monsterId, msg);

		}
		break;
		default:
		{
			std::cout << "Packet Error\n";
		}
		break;
		}
		_bufferQueue.pop();
	}
}

int SocketIO::GetNextId()
{
	// TODO: 임시로 해놨음
	if (_idList.size() <= _idCount) {
		return -1;
	}
	return _idList[_idCount++];
}

int SocketIO::GetMonsterId()
{
	if (_monsterIdList.size() <= _monsterIdCount) {
		return -1;
	}
	return _monsterIdList[_monsterIdCount++];
}

SocketIO::~SocketIO()
{
	if (_recvThread.joinable()) {
		_recvThread.join();  // 스레드 종료 대기
	}
	closesocket(_serverSocket);
	WSACleanup();
}