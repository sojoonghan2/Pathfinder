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
	if (not _stop) {
		ProcessPacket();
	}
}


void SocketIO::Continue()
{
	if (_stop) {
		_stop = false;
		auto msg{ std::make_shared<MsgStartGame>() };
		GET_SINGLE(MessageManager)->PushMessageByNetworkId(ID_RUINS_SCENE, msg);
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
		case packet::Type::SC_MATCHMAKING:
		{
			packet::SCMatchmaking& packet{ reinterpret_cast<packet::SCMatchmaking&>(buffer) };
			_myId = packet.playerId;
			_roomType = packet.roomType;

			// todo: 예외처리 필요한가?
			GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");
		}
		break;
		case packet::Type::SC_GAME_START:
		{
			// 게임 시작 메시지를 보내자
			// todo:
			// 일단 임시로 ruinsScene에 보냄

			// 아직 씬 전환이 완료가 안되었으면.
			if (not GET_SINGLE(MessageManager)->FindNetworkObject(ID_RUINS_SCENE)) {
				_stop = true;
			}
			else {
				// 신 전환이 이미 완료되었음.
				auto msg{ std::make_shared<MsgStartGame>() };
				GET_SINGLE(MessageManager)->PushMessageByNetworkId(ID_RUINS_SCENE, msg);
			}

		}
		break;
		case packet::Type::SC_MOVE_OBJECT:
		{
			packet::SCMoveObject& packet{ reinterpret_cast<packet::SCMoveObject&>(buffer) };
			
			if (not GET_SINGLE(MessageManager)->FindNetworkObject(packet.objectId)) {
				if (ObjectType::Player == packet.objectType && packet.objectId == _myId) {
					packet.objectType = ObjectType::MainPlayer;
				}
				GET_SINGLE(MessageManager)->AllocNetworkObject(packet.objectType, packet.objectId);
			}

			auto msg{ std::make_shared<MsgMove>(packet.x, packet.y, packet.dirX, packet.dirY) };
			GET_SINGLE(MessageManager)->PushMessageByNetworkId(packet.objectId, msg);
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


SocketIO::~SocketIO()
{
	if (_recvThread.joinable()) {
		_recvThread.join();  // 스레드 종료 대기
	}
	closesocket(_serverSocket);
	WSACleanup();
}