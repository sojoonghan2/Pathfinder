#include "pch.h"
#include "SocketIO.h"

#include "Input.h"
#include "Timer.h"
#include "MessageManager.h"
#include "SceneManager.h"


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
	ret = connect(_serverSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (SOCKET_ERROR == ret) {
		util::DisplayQuitError();
	}

	std::println("Connected to server successfully.");

	// ������ ����
	_recvThread = std::thread{ [this]() { Worker(); } };

	// �α��� �˸�
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
	// ���� ���� ���� �ʱ�ȭ
	BufferType buffer{};

	// ���� ���� RECV
	int recv_len = recv(
		_serverSocket,
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
				_serverSocket,
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
			// ���� ���� �޽����� ������
			// todo:
			// �ϴ� �ӽ÷� ruinsScene�� ����

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

			// todo: ����ó�� �ʿ��Ѱ�?
			GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");

			// todo:
			// �ε� ������ �ѱ��� �޽����� ������
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
	// TODO: �ӽ÷� �س���
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
		_recvThread.join();  // ������ ���� ���
	}
	closesocket(_serverSocket);
	WSACleanup();
}