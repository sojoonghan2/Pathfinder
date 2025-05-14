#include "pch.h"
#include "SocketIO.h"

#include "Input.h"
#include "Timer.h"
#include "MessageManager.h"
#include "SceneManager.h"
#include "LoadingScene.h"


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
	std::string server_ip;
	std::cin >> server_ip;

	ret = inet_pton(AF_INET, server_ip.c_str(), &serveraddr.sin_addr);
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

	_stop = false;
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
		auto msg{ std::make_shared<Msg>(MsgType::START_GAME) };
		GET_SINGLE(MessageManager)->PushMessageByNetworkId(ID_SCENE_RUINS, msg);
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
			shared_ptr<LoadingScene> loadingScene = make_shared<LoadingScene>();
			GET_SINGLE(SceneManager)->RegisterScene(L"LoadingScene", loadingScene);
			loadingScene->Init(RoomType::RUIN);
			GET_SINGLE(SceneManager)->LoadScene(L"LoadingScene");
		}
		break;
		case packet::Type::SC_GAME_START:
		{
			// ���� ���� �޽����� ������
			// todo:
			// �ϴ� �ӽ÷� ruinsScene�� ����

			// ���� �� ��ȯ�� �Ϸᰡ �ȵǾ�����.
			if (not GET_SINGLE(MessageManager)->FindNetworkObject(ID_SCENE_RUINS)) {
				_stop = true;
			}
			else {
				// �� ��ȯ�� �̹� �Ϸ�Ǿ���.
				auto msg{ std::make_shared<Msg>(MsgType::START_GAME) };
				GET_SINGLE(MessageManager)->PushMessageByNetworkId(ID_SCENE_RUINS, msg);
			}

		}
		break;
		case packet::Type::SC_MOVE_OBJECT:
		{
			packet::SCMoveObject& packet{ reinterpret_cast<packet::SCMoveObject&>(buffer) };
			
			if (not GET_SINGLE(MessageManager)->FindNetworkObject(packet.objectId)) {
				if (ObjectType::PLAYER == packet.objectType && packet.objectId == _myId) {
					packet.objectType = ObjectType::MAIN_PLAYER;
				}
				GET_SINGLE(MessageManager)->AllocNetworkObject(packet.objectType, packet.objectId);
			}

			auto msg{ std::make_shared<MsgMove>(packet.x, packet.y, packet.dirX, packet.dirY) };
			GET_SINGLE(MessageManager)->PushMessageByNetworkId(packet.objectId, msg);
		}
		break;
		case packet::Type::SC_DELETE_OBJECT:
		{
			packet::SCDeleteObject& packet{ reinterpret_cast<packet::SCDeleteObject&>(buffer) };
			GET_SINGLE(MessageManager)->DeleteNetworkObject(packet.objectId);
		}
		break;

		case packet::Type::SC_SET_OBJECT_HP:
		{
			packet::SCSetObjectHp& packet{ reinterpret_cast<packet::SCSetObjectHp&>(buffer) };
			auto msg{ std::make_shared<MsgSetObjectHp>(packet.hp, -1.f, _myId == packet.attackerId) };
			GET_SINGLE(MessageManager)->PushMessageByNetworkId(packet.objectId, msg);
			
			// �����ڰ� ���� ���.
			if (_myId == packet.attackerId) {
				_attackId = packet.objectId;
			}
			// �����ڰ� �ٸ��� �°� �ִ� ��ü�� ���� ���
			else if (_attackId == packet.objectId) {
				auto msg_other{ std::make_shared<MsgSetObjectHp>(packet.hp, -1.f, false) };
				GET_SINGLE(MessageManager)->PushMessageByNetworkId(ID_OBJECT_MONSTER_HP, msg_other);
			}

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