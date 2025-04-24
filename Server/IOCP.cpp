#include "pch.h"
#include "Player.h"

#include "IOCP.h"
#include "Game.h"
#include "Timer.h"

std::random_device rd;
std::default_random_engine dre{ rd() };
std::uniform_real_distribution<float> timeDist{ 2.f, 4.f };

bool IOCP::Init()
{
	// ������ �ʱ�ȭ
	WSADATA wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != ret) {
		util::DisplayError();
		return false;
	}

	// listen socket ���� �����
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	if (INVALID_SOCKET == _listenSocket) {
		util::DisplayError();
		return false;
	}

	// ���� �ּ� ����
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUMBER);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	// bind
	ret = bind(_listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (0 != ret) {
		util::DisplayError();
		return false;
	}

	// listen
	ret = listen(_listenSocket, SOMAXCONN);
	if (0 != ret) {
		util::DisplayError();
		return false;
	}

	// IOCP �ڵ� ����
	_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// ������ IOCP �ڵ��� listen socket�� ����
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_listenSocket), _IOCPHandle, 99999, 0);

	std::println("IOCP Init Successed.");

	return true;
}

bool IOCP::Start()
{
	// Ŭ���̾�Ʈ accept socket ����
	_acceptSocket = WSASocket(
		AF_INET,
		SOCK_STREAM,
		0,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED);
	int flag = 1;
	setsockopt(_acceptSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

	// accpet�� ���� OverlappedEx ����
	OverlappedEx accept_over_ex;
	accept_over_ex.operation = IOOperation::ACCEPT;
	accept_over_ex.clientSocket = _acceptSocket;

	int addr_size = sizeof(SOCKADDR_IN);
	DWORD bytes_received{};


	// listen_socket�� accept_socket �񵿱� Accept ���
	AcceptEx(
		_listenSocket,
		_acceptSocket,
		accept_over_ex.dataBuffer,
		0,
		addr_size + 16,
		addr_size + 16,
		&bytes_received,
		&accept_over_ex.over
	);


	// unsigned int num_thread{ std::thread::hardware_concurrency() };

	// �񵿱� IO �۾� �Ϸ� Ȯ�� ������ ����
	// worker_thread ���� ��������� ����

	int thread_number = std::thread::hardware_concurrency();
	for (int i = 0; i < 2; ++i) {
		_workers.emplace_back([this]() { Worker(); });
	}
	_workers.emplace_back([this]() { TimerWorker(); });
	std::cout << "Created " << thread_number << " Threads\n";

	return true;
}

void IOCP::Worker()
{
	while (true) {
		DWORD io_size;
		ULONG_PTR ULkey;
		WSAOVERLAPPED* over = nullptr;

		auto ret = GetQueuedCompletionStatus(
			_IOCPHandle,
			&io_size,
			&ULkey,
			&over,
			INFINITE);

		OverlappedEx* curr_over_ex = reinterpret_cast<OverlappedEx*>(over);
		
		// ���� volatile �ʿ��� ��ġ
		int key = static_cast<int>(ULkey);

		if (FALSE == ret) {
			// TODO: ERROR
			closesocket(_clientInfoHash[key].clientSocket);
			_clientInfoHash[key].ioState = IOState::DISCONNECT;
			// Send �ϰ�� ���� curr_ex�� ����
			if (curr_over_ex->operation == IOOperation::SEND) {
				delete curr_over_ex;
			}
		}


		// �Ϸ�� �۾��� OverlappedEx ������ �д´�.
		// � operation���� �Ϸ�Ǿ����� Ȯ��. 
		switch (curr_over_ex->operation) {

			/**G
			* ACCEPT: AcceptEx() �۾� �Ϸ�
			*/
		case IOOperation::ACCEPT:
		{
			int client_id = _sessionCnt++;
			_clientInfoHash.insert(std::make_pair(client_id, ClientInfo{}));
			_clientInfoHash[client_id].currentDataSize = 0;
			_clientInfoHash[client_id].clientSocket = _acceptSocket;
			_clientInfoHash[client_id].overEx.clientSocket = _acceptSocket;


			// IOCP ��ü�� �޾Ƶ��� Ŭ���̾�Ʈ�� ������ ����
			auto ret = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(_acceptSocket),
				_IOCPHandle,
				client_id,
				0);

			// WSARecv ȣ��.
			DoRecv(_clientInfoHash[client_id]);

			// accept�� ���� ���ο� ���� ����
			_acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			int flag = 1;
			setsockopt(_acceptSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

			// Accept�� overlapped ���� 
			ZeroMemory(&curr_over_ex->over, sizeof(curr_over_ex->over));
			curr_over_ex->clientSocket = _acceptSocket;
			int addr_size = sizeof(SOCKADDR_IN);
			DWORD bytes_received{};

			AcceptEx(
				_listenSocket,
				_acceptSocket,
				curr_over_ex->dataBuffer,
				0,
				addr_size + 16,
				addr_size + 16,
				& bytes_received,
					& curr_over_ex->over
					);
					break;
		}

		/**
		* OP_RECV
		*/
		case IOOperation::RECV:
		{
			while (_clientInfoHash.end() == _clientInfoHash.find(key)) { std::this_thread::yield(); }
			int remain_data = io_size + _clientInfoHash[key].currentDataSize;
			char* p = curr_over_ex->dataBuffer;

			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					ProcessPacket(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;

			}

			_clientInfoHash[key].currentDataSize = remain_data;
			if (remain_data > 0)
				memcpy(curr_over_ex->dataBuffer, p, remain_data);
			DoRecv(_clientInfoHash[key]);

			break;
		}


		/**
		* OP_SEND
		*/
		case IOOperation::SEND:
		{
			// ���� OVER_EXP �����.
			delete curr_over_ex;
			break;
		}
		}
	}
}

void IOCP::TimerWorker()
{
	using namespace std::chrono_literals;
	Timer update_timer;
	Timer send_timer;
	while (true) {

		update_timer.updateDeltaTime();
		auto delta = update_timer.getDeltaTimeSeconds();
		GET_SINGLE(Game)->Update(delta);


		// ������ ��ġ�� �÷��̾����� ����
		if (send_timer.PeekDeltaTime() > MOVE_PACKET_TIME_MS) {
			send_timer.updateDeltaTime();



			// ���� ���� Ȱ��ȭ �Ǿ��ִ��� Ȯ��
			// �ϴ� ŵ
			for (int i = 0; i < MAX_MONSTER; ++i) {
				const auto& monster{ GET_SINGLE(Game)->GetMonster(i) };
				auto room_id{ monster.GetRoomId() };
				if (-1 == room_id) {
					continue;
				}
				if (GET_SINGLE(Game)->GetRoom(monster.GetRoomId()).GetRoomStatus() != RoomStatus::Running) {
					continue;
				}

				auto monster_pos = monster.GetPos();
				auto monster_dir = monster.GetDir();

				// ��Ŷ ����
				// �ϴ� �ӽ÷� MAX_PLAYER�� ���� ���̵� �ش�.
				// TODO: ������Ʈ�� �� �ڷᱸ����(shared_ptr) �����ϰ� �Ǹ� id���� ���� ��.
				packet::SCMoveMonster sc_monster_move{
					MAX_PLAYER + i,
					monster_pos.x,
					monster_pos.y,
					monster_dir.x,
					monster_dir.y
				};
				// �濡 �ִ� �÷��̾�� ����
				auto list{ _roomInfoList[monster.GetRoomId()].GetClientIdList() };
				for (auto id : list) {
					if (id == -1) {
						continue;
					}
					if (_clientInfoHash[id].ioState != IOState::INGAME) {
						continue;
					}
					DoSend(id, &sc_monster_move);
				}

			}
		}
	}
}

void IOCP::DoRecv(ClientInfo& session) const
{

	DWORD recv_flag = 0;
	OverlappedEx& over_ex = session.overEx;
	ZeroMemory(&over_ex, sizeof(over_ex));

	// ���� ���� ��ŭ recv�Ѵ�. 
	over_ex.wsabuf.len = BUFFER_SIZE - session.currentDataSize;
	over_ex.wsabuf.buf = over_ex.dataBuffer + session.currentDataSize;
	over_ex.operation = IOOperation::RECV;

	// �񵿱� Recv
	WSARecv(
		session.clientSocket,
		&over_ex.wsabuf,
		1,
		0,
		&recv_flag,
		&over_ex.over,
		0);
}

void IOCP::DoSend(ClientInfo& client_info, void* packet)
{
	OverlappedEx* send_over_ex = new OverlappedEx{ reinterpret_cast<unsigned char*>(packet) };
	WSASend(client_info.clientSocket, &send_over_ex->wsabuf, 1, 0, 0, &send_over_ex->over, 0);
}

void IOCP::DoSend(int client_id, void* packet)
{
	DoSend(_clientInfoHash[client_id], packet);
}

void IOCP::ProcessPacket(int key, char* p)
{

	// IOCP���� ó��
	packet::Header* header = reinterpret_cast<packet::Header*>(p);
	switch (header->type)
	{
	case packet::Type::CS_LOGIN:
	{
		packet::SCLogin sc_login{};
		DoSend(_clientInfoHash[key], &sc_login);
	}
	break;

	case packet::Type::CS_MATCHMAKING:
	{
		// Ŭ���̾�Ʈ�� ��ġ����ŷ ��Ŷ�� �����Ͽ��� ���
		// Ŭ���̾�Ʈ ���̵� ť�� �ְ� ������ �˻�
		// ����� 3�̻��̸� Ŭ���̾�Ʈ 3���� �̾Ƴ�.

		// ���� Ŭ���̾�Ʈ 3���� �̴µ� �����Ѵٸ�
		// ���� ���� Ŭ���̾�Ʈ id�� �켱������ ������ �ٽ� ��ġ����ŷ ť�� ����

		// ��ȣ 3���� �̴µ� �����Ѵٸ� 
		// �÷��̾� ��ȣ�� 3�� �÷��̾� ť���� ����.
		// ���� ��ȣ 3���� ���� Ŭ���̾�Ʈ�� �ο�
		// �� ��ȣ ť���� ��ȣ�� �ϳ��� ������
		// �� ��ȣ�� �濡 �÷��̾� ���̵� ����.

		// TODO:
		// ����ȭ �� ��: ��ġ����ŷ ��Ŷ�� ������ ������ ©��� �Ѵ�.
		// atomic�� STATE ������ �ʿ��� ����
		
		// ��ġ����ŷ �̷��Ÿ� �׳� ������.. �װ� ���ڴ�
	

		// �ϴ� ���̵� �ִ´�.
		_matchmakingQueue.push(key);

		// 3 �̸��̸� �׳� ������
		if (_matchmakingQueue.unsafe_size() < 3) {
			break;
		}


		// ���� Ŭ���̾�Ʈ�� 3 �̻��̸� ������ ���� ��ġ����ŷ�� ����
		// ��Ī ���н� ����� �ð�
		float waiting_time = timeDist(dre);

		// ť���� �̾Ƴ� 3���� Ŭ���̾�Ʈ ���̵� ������ �迭
		std::array<int, 3> client_ids{};
		client_ids.fill(-1);

		bool loop{ false };
		bool match{ false };
		Timer pop_timer;
		int count{ 1 };

		// ��ġ����ŷ �˰���
		do {
			loop = false;

			// ��ġ����ŷ ť�� ������ �� ��� 3���� ����
			if (_matchmakingQueue.unsafe_size() >= 3) {
				for (auto& id : client_ids) {
					if (not _matchmakingQueue.try_pop(id)) {
						loop = true;
						break;
					}
				}
				match = true;
			}

			// �����ϸ� �ٽ� �ְ� ���ð��� ���� �� �÷����鼭 ��õ�
			if (loop) {

				// �ٽ� ����
				for (auto& id : client_ids) {
					if (-1 != id) {
						_matchmakingQueue.push(id);
					}
				}

				// ��õ�
				pop_timer.updateDeltaTime();
				while (pop_timer.PeekDeltaTime() < waiting_time) {
					std::this_thread::yield();
				}
				waiting_time += timeDist(dre) * count++;
			}
		} while (loop);



		// -- Ŭ���̾�Ʈ ���� ��Ī �Ϸ� --

		// �� ��ȣ�� ����
		int room_id{ -1 };
		for (int i{}; i < MAX_ROOM; ++i) {
			if (not _roomInfoList[i].GetRunning()) {
				if (_roomInfoList[i].CASRunning(false, true)) {
					// �� ��ȣ ��� ����
					room_id = i;
					break;
				}
			}
			
		}

		// �� ��ȣ�� ������ �� ������.
		if (-1 == room_id) {
			// TODO: ���� �κ��� ����ȭ
			// �ϴ� �ӽ÷� �ٽ� �ְ� ����.
			for (auto& id : client_ids) {
				if (-1 != id) {
					_matchmakingQueue.push(id);
				}
			}
			break;
		}

		// �÷��̾� ���̵�� �� ��ȣ�� ���� ����.
		std::array<int, 3> player_ids{
			room_id * 3,
			room_id * 3 + 1,
			room_id * 3 + 2
		};

		// -- �� ��ȣ�� �÷��̾� ��ȣ�� ������. -- 
		// ���� ������ �������� ����
		// Room, RoomInfo, Player���ٰ� ������ ����

		// ���� �� �ʱ�ȭ. ��, �÷��̾�, ������ ���� ������ �ʱ�ȭ���ش�.
		GET_SINGLE(Game)->InitRoom(room_id);
		
		// roominfo�� �ε� �Ϸ�� �÷��̾� 0������ ����
		_roomInfoList[room_id].SetLoadingCount(0);

		for (int i = 0; i < 3; ++i) {
			// RoomInfo�� ������ ����
			_roomInfoList[room_id].InsertClient(i, client_ids[i]);

			// ClientInfoHash�� ������ ����
			_clientInfoHash[client_ids[i]].clientIdInfo.roomId = room_id;
			_clientInfoHash[client_ids[i]].clientIdInfo.playerId = player_ids[i];
			_clientInfoHash[client_ids[i]].ioState = IOState::INGAME;
		}

		// SC_MATCHMAKING ��Ī �Ϸ� �˸� �� �ε��� ���� Ÿ���� ��.
		auto room_type{ GET_SINGLE(Game)->GetRoom(room_id).GetRoomType() };
		
		// Ŭ���̾�Ʈ �ε� ����
		for (int i = 0; i < 3; ++i) {
			packet::SCMatchmaking matchmaking_packet{ client_ids[i], room_type };
			DoSend(client_ids[i], &matchmaking_packet);
		}


	
	}
	break;

	case packet::Type::CS_LOAD_COMPLETE:
	{
		// TODO:
		// ���� ó�� ���

		// ��� �Ϸ� �÷��̾� ���� ����
		auto room_id{ _clientInfoHash[key].clientIdInfo.roomId };
		
		// Ȥ�� �� ����ó��
		if (false == _roomInfoList[room_id].GetRunning()) {
			Disconnect(key);
		}

		// �÷��̾� �ε� ��Ȳ ����
		bool load_complete{ false };
		while (true) {
			auto targ{ _roomInfoList[room_id].GetLoadingCount() };
			if (_roomInfoList[room_id].CASLoadingCount(targ, targ + 1)) {
				if (3 == targ + 1) {
					load_complete = true;
				}
				break;
			}
		}

		// ���� �ε��� �ȵ���
		if (not load_complete) {
			break;
		}

		// �ε� �Ϸ�
		// ������ �÷��̾ �� �ε��� �Ϸ�Ǿ��ٸ�
		// ���� ���¸� RUNNING���� �ϰ� ( room update�� ���̴� ���� )
		// ���� ���� ��Ŷ�� ���� ��.

		// �ε� ī��Ʈ �ʱ�ȭ.
		_roomInfoList[room_id].SetLoadingCount(0);

		// Ŭ���̾�Ʈ ���̵�� �÷��̾� ���̵� �����´�.
		auto client_ids{ _roomInfoList[room_id].GetClientIdList() };
		std::array<int, 3> player_ids{
			room_id * 3,
			room_id * 3 + 1,
			room_id * 3 + 2
		};


		// �濡 �ִ� �÷��̾�鿡�� ��ġ�� ����
		for (int i{}; i < 3; ++i) {

			auto& player = GET_SINGLE(Game)->GetPlayer(player_ids[i]);
			auto pos = player.GetPos();
			auto dir = player.GetDir();

			packet::SCGameStart sc_game_start{};
			DoSend(client_ids[i], &sc_game_start);

			packet::SCMovePlayer sc_move_player{ client_ids[i], pos.x, pos.y, dir.x, dir.y };

			for (auto other_id : client_ids) {
				DoSend(other_id, &sc_move_player);
			}
		}

		// ���� ����. �� ���¸� Running����.
		GET_SINGLE(Game)->GetRoom(room_id).SetRoomStatus(RoomStatus::Running);
				

	}
	break;

	case packet::Type::CS_MOVE_PLAYER:
	{
		packet::CSMovePlayer* packet{ reinterpret_cast<packet::CSMovePlayer*>(p) };
		if (_clientInfoHash[key].ioState != IOState::INGAME) {
			break;
		}

		auto& id_info{ _clientInfoHash[key].clientIdInfo };

		// ���� ��ġ�� ����
		auto& player = GET_SINGLE(Game)->GetPlayer(id_info.playerId);
		player.SetDir(Vec2f{ packet->dirX, packet->dirY });
		player.Move(packet->x, packet->y);

		auto pos = player.GetPos();
		auto dir = player.GetDir();

		// �ٸ� �÷��̾�� ����
		packet::SCMovePlayer send_packet{ key, pos.x, pos.y, dir.x, dir.y };
		auto other_clients{ _roomInfoList[id_info.roomId].GetClientIdList() };

		for (auto other : other_clients) {
			if (other == -1) { continue; }
			if (_clientInfoHash[other].ioState != IOState::INGAME ||
				other == key) {
				continue;
			}
			DoSend(_clientInfoHash[other], &send_packet);
		}
	}
	break;

	case packet::Type::CS_CHECK_DELAY:
	{

		packet::SCCheckDelayPacket send_packet{};
		DoSend(_clientInfoHash[key], &send_packet);


	}
	break;
	

	default:
	{
		std::println("packet Error. disconnect Client {}", key);

		// todo: ��Ŷ ������ Ŭ���̾�Ʈ ����
		_clientInfoHash[key].ioState = IOState::DISCONNECT;
		closesocket(_clientInfoHash[key].clientSocket);

		return;
	}
	}


}

void IOCP::DoBroadcast(void* packet)
{
	// todo: ����
	for (int i = 0; i < _sessionCnt; ++i) {
		DoSend(_clientInfoHash[i], packet);
	}
}

void IOCP::DoBroadcast(int key, void* packet)
{
	// todo: ����
	for (int i = 0; i < _sessionCnt; ++i) {
		if (key == i) continue;
		DoSend(_clientInfoHash[i], packet);
	}
}

void IOCP::Disconnect(int client_id)
{
	if (INVALID_SOCKET == _clientInfoHash[client_id].clientSocket) {
		closesocket(_clientInfoHash[client_id].clientSocket);
	}
	// Todo:
	// ���߿� ���� atomic_shared_ptr�� �ڵ����� ���� ����.
}



IOCP::~IOCP()
{
	for (auto& thread : _workers) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	closesocket(_listenSocket);
	WSACleanup();
}
