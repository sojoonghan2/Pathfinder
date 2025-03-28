#include "pch.h"
#include "Player.h"

#include "IOCP.h"
#include "Game.h"

bool IOCP::Init()
{
	// 윈도우 초기화
	WSADATA wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != ret) {
		util::DisplayError();
		return false;
	}

	// listen socket 소켓 만들기
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	if (INVALID_SOCKET == listenSocket) {
		util::DisplayError();
		return false;
	}

	// 소켓 주소 설정
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUMBER);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	// bind
	ret = bind(listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (0 != ret) {
		util::DisplayError();
		return false;
	}

	// listen
	ret = listen(listenSocket, SOMAXCONN);
	if (0 != ret) {
		util::DisplayError();
		return false;
	}

	// IOCP 핸들 생성
	IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// 생성한 IOCP 핸들을 listen socket에 연결
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), IOCPHandle, 99999, 0);

	std::println("IOCP Init Successed.");

	return true;
}

bool IOCP::Start()
{
	// 클라이언트 accept socket 생성
	acceptSocket = WSASocket(
		AF_INET,
		SOCK_STREAM,
		0,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED);
	int flag = 1;
	setsockopt(acceptSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

	// accpet를 위한 OverlappedEx 생성
	OverlappedEx accept_over_ex;
	accept_over_ex.operation = IOOperation::ACCEPT;
	accept_over_ex.clientSocket = acceptSocket;

	int addr_size = sizeof(SOCKADDR_IN);
	DWORD bytes_received{};


	// listen_socket에 accept_socket 비동기 Accept 등록
	AcceptEx(
		listenSocket,
		acceptSocket,
		accept_over_ex.dataBuffer,
		0,
		addr_size + 16,
		addr_size + 16,
		&bytes_received,
		&accept_over_ex.over
	);


	// unsigned int num_thread{ std::thread::hardware_concurrency() };

	// 비동기 IO 작업 완료 확인 스레드 생성
	// worker_thread 따로 멤버변수로 뺴기

	int thread_number = std::thread::hardware_concurrency();
	for (int i = 0; i < thread_number; ++i) {
		workers.emplace_back([this]() { Worker(); });
	}
	workers.emplace_back([this]() { TimerWorker(); });
	std::cout << "Created " << thread_number << " Threads\n";

	return true;
}

void IOCP::SetClientIdInfo(int client_id, int player_id, int room_id)
{
	clientInfoHash[client_id].clientIdInfo.playerId = player_id;
	clientInfoHash[client_id].clientIdInfo.roomId = room_id;
	clientInfoHash[client_id].ioState = IOState::INGAME;
}

IOState IOCP::GetClientIOState(int client_id)
{
	return clientInfoHash[client_id].ioState;
}

void IOCP::Worker()
{
	while (true) {
		DWORD io_size;
		ULONG_PTR ULkey;
		WSAOVERLAPPED* over = nullptr;

		auto ret = GetQueuedCompletionStatus(
			IOCPHandle,
			&io_size,
			&ULkey,
			&over,
			INFINITE);

		OverlappedEx* curr_over_ex = reinterpret_cast<OverlappedEx*>(over);
		int key = static_cast<int>(ULkey);

		if (FALSE == ret) {
			// TODO: ERROR
			closesocket(clientInfoHash[key].clientSocket);
			clientInfoHash[key].ioState = IOState::DISCONNECT;
			// Send 일경우 보낸 curr_ex는 제거
			if (curr_over_ex->operation == IOOperation::SEND) {
				delete curr_over_ex;
			}
		}


		// 완료된 작업의 OverlappedEx 정보를 읽는다.
		// 어떤 operation으로 완료되었는지 확인. 
		switch (curr_over_ex->operation) {

			/**
			* ACCEPT: AcceptEx() 작업 완료
			*/
		case IOOperation::ACCEPT:
		{
			int client_id = sessionCnt++;
			clientInfoHash.insert(std::make_pair(client_id, ClientInfo{}));
			clientInfoHash[client_id].currentDataSize = 0;
			clientInfoHash[client_id].clientSocket = acceptSocket;
			clientInfoHash[client_id].overEx.clientSocket = acceptSocket;


			// IOCP 객체에 받아들인 클라이언트의 소켓을 연결
			auto ret = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(acceptSocket),
				IOCPHandle,
				client_id,
				0);

			// WSARecv 호출.
			DoRecv(clientInfoHash[client_id]);

			// accept를 위한 새로운 소켓 생성
			acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			int flag = 1;
			setsockopt(acceptSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

			// Accept로 overlapped 설정 
			ZeroMemory(&curr_over_ex->over, sizeof(curr_over_ex->over));
			curr_over_ex->clientSocket = acceptSocket;
			int addr_size = sizeof(SOCKADDR_IN);
			DWORD bytes_received{};

			AcceptEx(
				listenSocket,
				acceptSocket,
				curr_over_ex->dataBuffer,
				0,
				addr_size + 16,
				addr_size + 16,
				&bytes_received,
				&curr_over_ex->over
			);
			break;
		}

		/**
		* OP_RECV
		*/
		case IOOperation::RECV:
		{
			while (clientInfoHash.end() == clientInfoHash.find(key)) { std::this_thread::yield(); }
			int remain_data = io_size + clientInfoHash[key].currentDataSize;
			char* p = curr_over_ex->dataBuffer;

			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					bool ret = ProcessPacket(static_cast<int>(key), p);
					if (false == ret) {
						// Todo
						closesocket(clientInfoHash[key].clientSocket);
						clientInfoHash[key].ioState = IOState::DISCONNECT;
						
						break;
					}
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;

			}

			clientInfoHash[key].currentDataSize = remain_data;
			if (remain_data > 0)
				memcpy(curr_over_ex->dataBuffer, p, remain_data);
			DoRecv(clientInfoHash[key]);

			break;
		}


		/**
		* OP_SEND
		*/
		case IOOperation::SEND:
		{
			// 보낸 OVER_EXP 지우기.
			delete curr_over_ex;
			break;
		}
		}
	}
}

void IOCP::TimerWorker()
{
	using namespace std::chrono_literals;
	while (true) {

		std::vector<int> id_delete{};
		for (auto& [id, client_info] : clientInfoHash) {

			// 인게임 체크해서 게임중이 아니면 지우기.
			if (client_info.ioState == IOState::DISCONNECT) {
				id_delete.push_back(id);
				continue;
			}

			//// 내 위치를 가져와서 패킷을 만듦
			//if (client_info.ioState != IOState::INGAME) {
			//	continue;
			//}

			//auto pos = GET_SINGLE(Game)->GetPlayerPosition(client_info.clientIdInfo.playerId);
			//packet::SCMovePlayer packet{ id, pos.x, pos.y };

			//// 내 방에 있는 플레이어에게 패킷을 보냄
			//auto other_players = GET_SINGLE(Game)->GetRoomClients(client_info.clientIdInfo.roomId);
			//for (auto other : other_players) {
			//	if (other == -1) { continue; }
			//	if (clientInfoHash[other].ioState != IOState::INGAME ||
			//		other == id) {
			//		continue;
			//	}
			//	DoSend(clientInfoHash[other], &packet);
			//}

		}

		std::this_thread::sleep_for(1s);

		// 더이상 사용되지 않는 id는 제거
		for (auto id : id_delete) {
			clientInfoHash.unsafe_erase(id);
		}
	}
}

void IOCP::DoRecv(ClientInfo& session) const
{

	DWORD recv_flag = 0;
	OverlappedEx& over_ex = session.overEx;
	ZeroMemory(&over_ex, sizeof(over_ex));

	// 현재 남은 만큼 recv한다. 
	over_ex.wsabuf.len = BUFFER_SIZE - session.currentDataSize;
	over_ex.wsabuf.buf = over_ex.dataBuffer + session.currentDataSize;
	over_ex.operation = IOOperation::RECV;

	// 비동기 Recv
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

bool IOCP::ProcessPacket(int key, char* p)
{
	packet::Header* header = reinterpret_cast<packet::Header*>(p);
	switch (header->type)
	{
	case packet::Type::CS_LOGIN:
	{
		packet::SCLogin sc_login{ key };
		DoSend(clientInfoHash[key], &sc_login);

		// 매치메이킹 등록 및 방 생성 함수
		GET_SINGLE(Game)->RegisterClient(key);
		
		// 매치매이킹이 아직 안되었으면 빠져나가기
		if (clientInfoHash[key].ioState != IOState::INGAME) {
			break;
		}

		// 매칭이 완료되면 본인 위치를 본인 클라이언트에 전달
		auto client_ids = GET_SINGLE(Game)->GetRoomClients(
			clientInfoHash[key].clientIdInfo.roomId);

		for (auto id : client_ids) {
			auto pos = GET_SINGLE(Game)->GetPlayerPosition(
				clientInfoHash[id].clientIdInfo.playerId);
			packet::SCMovePlayer sc_move_player{
				id, pos.x, pos.y };
			DoSend(clientInfoHash[id], &sc_move_player);
		}
	}
	break;

	case packet::Type::CS_MOVE_PLAYER:
	{
		packet::CSMovePlayer* packet = reinterpret_cast<packet::CSMovePlayer*>(p);
		if (clientInfoHash[key].ioState != IOState::INGAME) {
			break;
		}

		GET_SINGLE(Game)->SetPlayerPosition(
			key,
			Vec2f{ packet->x, packet->y });

		auto pos = GET_SINGLE(Game)->GetPlayerPosition(clientInfoHash[key].clientIdInfo.playerId);
		packet::SCMovePlayer send_packet{ key, pos.x, pos.y };

		// 내 방에 있는 플레이어에게 패킷을 보냄
		auto other_players = GET_SINGLE(Game)->GetRoomClients(clientInfoHash[key].clientIdInfo.roomId);
		for (auto other : other_players) {
			if (other == -1) { continue; }
			if (clientInfoHash[other].ioState != IOState::INGAME ||
				other == key) {
				continue;
			}
			DoSend(clientInfoHash[other], &send_packet);
		}
	}
	break;

	case packet::Type::CS_CHECK_DELAY:
	{

		packet::SCCheckDelayPacket send_packet{};
		DoSend(clientInfoHash[key], &send_packet);


	}
	break;

	default:
	{
		std::println("packet Error. disconnect Client {}", key);
		return false;
	}
	}
	return true;
}

void IOCP::DoBroadcast(void* packet)
{
	// todo: 수정
	for (int i = 0; i < sessionCnt; ++i) {
		DoSend(clientInfoHash[i], packet);
	}
}

void IOCP::DoBroadcast(int key, void* packet)
{
	// todo: 수정
	for (int i = 0; i < sessionCnt; ++i) {
		if (key == i) continue;
		DoSend(clientInfoHash[i], packet);
	}
}

void IOCP::Disconnect(int client_id)
{
	
}



IOCP::~IOCP()
{
	for (auto& thread : workers) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	closesocket(listenSocket);
	WSACleanup();
}
