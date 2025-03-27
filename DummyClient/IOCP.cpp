#include "pch.h"
#include "IOCP.h"

bool IOCP::Init()
{
	// 윈도우 초기화
	WSADATA wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != ret) {
		util::DisplayError();
		return false;
	}

	// IOCP 핸들 생성
	IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	int thread_number = std::thread::hardware_concurrency();
	for (int i = 0; i < thread_number / 2; ++i) {
		workers.emplace_back([this]() { Worker(); });
	}

	workers.emplace_back([this]() { TimerWorker(); });
	workers.emplace_back([this]() { LoginWorker(); });


	std::cout << "IOCP INIT success.\n";
	return true;
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

			// TODO: make Disconnect
			closesocket(players[key].clientSocket);

			if (curr_over_ex->operation == IOOperation::SEND) {
				delete curr_over_ex;
			}
		}


		// 완료된 작업의 OverlappedEx 정보를 읽는다.
		// 어떤 operation으로 완료되었는지 확인. 
		switch (curr_over_ex->operation) {
		/**
		* OP_RECV
		*/
		case IOOperation::RECV:
		{
			int remain_data = io_size + players[key].currentDataSize;
			char* p = curr_over_ex->dataBuffer;

			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					bool ret = ProcessPacket(static_cast<int>(key), p);
					if (false == ret) {
						// Todo
						closesocket(players[key].clientSocket);
						players[key].ioState = IOState::DISCONNECT;
						break;
					}
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;

			}

			players[key].currentDataSize = remain_data;
			if (remain_data > 0)
				memcpy(curr_over_ex->dataBuffer, p, remain_data);
			DoRecv(players[key]);

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

	std::println("Worker Ended.");
}

void IOCP::TimerWorker()
{
	std::chrono::duration<float> time_float{ MOVE_PACKET_TIME_MS };
	auto time_milli{ std::chrono::duration_cast<std::chrono::milliseconds>(time_float) };
	while (true) {
		maxDelay = 0.f;
		auto next_time = std::chrono::high_resolution_clock::now() + time_milli;
		for (int i = 0; i < currentClient; ++i) {
			packet::CSMovePlayer move_packet{0.f, 0.f};
			DoSend(players[i], &move_packet);
			packet::CSCheckDelayPacket delay_packet;
			DoSend(players[i], &delay_packet);

		}
		std::this_thread::sleep_until(next_time);
	}

}

int IOCP::GetDelayTime() const
{
	float val = maxDelay.load();
	return static_cast<int>(val);
}

int IOCP::GetPlayerCount() const
{
	return currentClient.load();
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
		players[key].ioState = IOState::INGAME;	
	}
	break;

	case packet::Type::CS_MOVE_PLAYER:
	{

	}
	break;

	case packet::Type::CS_CHECK_DELAY:
	{
		auto tp = std::chrono::high_resolution_clock::now();
		auto delay = std::chrono::duration_cast
			<std::chrono::milliseconds>(
				delays[key] - tp).count();
		
		while (maxDelay < delay) {
			float val = maxDelay;
			if (maxDelay.compare_exchange_strong(val, delay)) { break; }
		}

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


void IOCP::Disconnect(const int client_id)
{

}

void IOCP::LoginWorker()
{	
	for (int i = 0; i < 200; ++i) {
		players[i].clientSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		int flag = 1;
		setsockopt(players[i].clientSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(SOCKADDR_IN));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(PORT_NUMBER);
		int ret = inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

		int Result = WSAConnect(players[i].clientSocket, (sockaddr*)&server_addr, sizeof(server_addr), NULL, NULL, NULL, NULL);
		if (0 != Result) {
			util::DisplayError();
		}

		players[i].ioState = IOState::CONNECT;

		DWORD recv_flag = 0;
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(players[i].clientSocket), IOCPHandle, i, 0);

		packet::CSLogin login_packet;
		DoSend(players[i], &login_packet);
		currentClient = i;
	}
}

IOCP::~IOCP()
{
	for (auto& thread : workers) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	
	for (auto& player : players) {
		if (player.ioState != IOState::DISCONNECT) {
			closesocket(player.clientSocket);
		}
	}

	WSACleanup();
}
