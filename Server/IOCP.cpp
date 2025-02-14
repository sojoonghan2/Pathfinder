#include "pch.h"
#include "IOCP.h"

bool IOCP::InitServer()
{
	// 윈도우 초기화
	WSADATA wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != ret) {
		std::println("Error: WSAStartup() Failed.");
		return false;
	}
	std::println("WSAStartup() Successed.");

	// listen socket 소켓 만들기
	SOCKET listen_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == listen_socket) {
		std::println("Error: Failed to create listen socket.");
		return false;
	}
	std::println("Created listen socket.");

	// 소켓 주소 설정
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	// bind
	ret = bind(listen_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (0 != ret) {
		std::println("Error: Failed to bind socket.");
		return false;
	}
	std::println("Bind Successed.");

	// listen
	ret = listen(listen_socket, SOMAXCONN);
	if (0 != ret) {
		std::println("Error: Failed to listen.");
		return false;
	}
	std::println("Listen Successed.");

	// IOCP 핸들 생성
	IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// 생성한 IOCP 핸들을 listen socket에 연결
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listen_socket), IOCPHandle, 9999, 0);

	return true;
}

bool IOCP::StartServer()
{
	// 클라이언트 accept socket 생성
	SOCKET accept_socket = WSASocket(
		AF_INET,
		SOCK_STREAM,
		0,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED);

	// accpet를 위한 OverlappedEx 생성
	OverlappedEx accept_over_ex;
	accept_over_ex.operation = IOOperation::ACCEPT;
	accept_over_ex.clientSocket = accept_socket;

	int addr_size = sizeof(SOCKADDR_IN);
	DWORD bytes_received{};

	// listen_socket에 accept_socket 비동기 Accept 등록
	AcceptEx(
		listenSocket,
		accept_socket,
		accept_over_ex.dataBuffer,
		0,
		addr_size + 16,
		addr_size + 16,
		&bytes_received,
		&accept_over_ex.over
	);

	std::println("AcceptEx Successed.");


	// unsigned int num_thread{ std::thread::hardware_concurrency() };

	// 비동기 IO 작업 완료 확인 스레드 생성
	std::vector<std::thread> worker_thread;
	for (int i = 0; i < 1; ++i) {
		worker_thread.emplace_back([this]() { worker(); });
	}
	std::println("Created Worker Thread.");


	for (auto& thread : worker_thread) {
		thread.join();
	}

	return true;
}

void IOCP::worker()
{
	std::println("Hello worker!");

	while (true) {
		DWORD io_size;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;

		auto ret = GetQueuedCompletionStatus(
			IOCPHandle,
			&io_size,
			&key,
			&over,
			INFINITE);

		if (FALSE == ret) {
			std::println("Client Error.\n {}", GetLastError());
			exit(-1);
		}

		OverlappedEx* curr_over_ex = reinterpret_cast<OverlappedEx*>(over);


		// 완료된 작업의 OverlappedEx 정보를 읽는다.
		// 어떤 operation으로 완료되었는지 확인. 
		switch (curr_over_ex->operation) {

			/**
			* ACCEPT: AcceptEx() 작업 완료
			*/
		case IOOperation::ACCEPT:
		{
			// 세션 컨테이너에 소켓 정보 저장
			int client_id = sessionCnt++;
			std::println("New Client {} Accepted.", client_id);
			SOCKET curr_socket = curr_over_ex->clientSocket;
			sessionList[client_id].clientSocket = curr_socket;

			// IOCP 객체에 받아들인 클라이언트의 소켓을 연결
			auto ret = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(curr_socket),
				IOCPHandle,
				client_id,
				0);

			// WSARecv 호출.


			// accept를 위한 새로운 소켓 생성
			curr_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

			// Accept로 overlapped 설정 
			ZeroMemory(&curr_over_ex->over, sizeof(curr_over_ex->over));
			curr_over_ex->clientSocket = curr_socket;
			curr_over_ex->operation = IOOperation::ACCEPT;
			int addr_size = sizeof(SOCKADDR_IN);
			DWORD bytes_received{};

			AcceptEx(
				listenSocket,
				curr_socket,
				curr_over_ex->dataBuffer,
				0,
				addr_size + 16,
				addr_size + 16,
				&bytes_received,
				&curr_over_ex->over
			);
			std::println("AcceptEx Successed.");
			break;
		}

		/**
		* OP_RECV
		*/
		case IOOperation::RECV:
		{
			// 패킷 재조립
			//int remain_data = num_bytes + clients[key]._prev_remain;
			//char* p = ex_over->_send_buf;
			//while (remain_data > 0) {
			//	int packet_size = p[0];
			//	if (packet_size <= remain_data) {
			//		process_packet(static_cast<int>(key), p);
			//		p = p + packet_size;
			//		remain_data = remain_data - packet_size;
			//	}
			//	else break;
			//}
			//clients[key]._prev_remain = remain_data;
			//if (remain_data > 0)
			//	memcpy(ex_over->_send_buf, p, remain_data);
			//clients[key].do_recv();
			//break;
			break;
		}


		/**
		* OP_SEND
		*/
		case IOOperation::SEND:
		{
			// 보낸 OVER_EXP 지우기.
			// delete ex_over;
			break;
		}
		}
	}
}


IOCP::~IOCP()
{
	closesocket(listenSocket);
	WSACleanup();
}
