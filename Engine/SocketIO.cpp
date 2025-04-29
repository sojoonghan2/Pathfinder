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
		case packet::Type::SC_MATCHMAKING:
		{
			packet::SCMatchmaking& packet{ reinterpret_cast<packet::SCMatchmaking&>(buffer) };
			_myId = packet.playerId;
			_roomType = packet.roomType;

			// todo: ����ó�� �ʿ��Ѱ�?
			GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");
		}
		break;
		case packet::Type::SC_GAME_START:
		{
			// ���� ���� �޽����� ������
			// todo:
			// �ϴ� �ӽ÷� ruinsScene�� ����

			// ���� �� ��ȯ�� �Ϸᰡ �ȵǾ�����.
			if (not GET_SINGLE(MessageManager)->FindNetworkObject(ID_RUINS_SCENE)) {
				_stop = true;
			}
			else {
				// �� ��ȯ�� �̹� �Ϸ�Ǿ���.
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
		_recvThread.join();  // ������ ���� ���
	}
	closesocket(_serverSocket);
	WSACleanup();
}