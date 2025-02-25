#include "pch.h"
#include "IOCP.h"

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
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == listenSocket) {
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

	// IOCP �ڵ� ����
	IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// ������ IOCP �ڵ��� listen socket�� ����
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), IOCPHandle, 9999, 0);

	std::println("IOCP Init Successed.");

	return true;
}

bool IOCP::Start()
{
	// Ŭ���̾�Ʈ accept socket ����
	SOCKET accept_socket = WSASocket(
		AF_INET,
		SOCK_STREAM,
		0,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED);

	// accpet�� ���� OverlappedEx ����
	OverlappedEx accept_over_ex;
	accept_over_ex.operation = IOOperation::ACCEPT;
	accept_over_ex.clientSocket = accept_socket;

	int addr_size = sizeof(SOCKADDR_IN);
	DWORD bytes_received{};

	// listen_socket�� accept_socket �񵿱� Accept ���
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


	// unsigned int num_thread{ std::thread::hardware_concurrency() };

	// �񵿱� IO �۾� �Ϸ� Ȯ�� ������ ����
	// worker_thread ���� ��������� ����
	std::vector<std::thread> worker_thread;
	for (int i = 0; i < 1; ++i) {
		worker_thread.emplace_back([this]() { Worker(); });
	}
	std::println("Created Worker Thread.");


	for (auto& thread : worker_thread) {
		thread.join();
	}

	return true;
}

void IOCP::Worker()
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
			util::DisplayQuitError();
		}

		OverlappedEx* curr_over_ex = reinterpret_cast<OverlappedEx*>(over);


		// �Ϸ�� �۾��� OverlappedEx ������ �д´�.
		// � operation���� �Ϸ�Ǿ����� Ȯ��. 
		switch (curr_over_ex->operation) {

			/**
			* ACCEPT: AcceptEx() �۾� �Ϸ�
			*/
		case IOOperation::ACCEPT:
		{
			// ���� �����̳ʿ� ���� ���� ����
			int client_id = sessionCnt++;
			std::println("New Client {} Accepted.", client_id);
			SOCKET curr_socket = curr_over_ex->clientSocket;
			sessionList[client_id].clientSocket = curr_socket;

			// IOCP ��ü�� �޾Ƶ��� Ŭ���̾�Ʈ�� ������ ����
			auto ret = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(curr_socket),
				IOCPHandle,
				client_id,
				0);

			// WSARecv ȣ��.
			DoRecv(sessionList[client_id]);

			// accept�� ���� ���ο� ���� ����
			curr_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

			// Accept�� overlapped ���� 
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

			std::println("Temp Send.");
			packet::SCLogin temp_packet;
			DoSend(sessionList[client_id], &temp_packet);
			break;
		}

		/**
		* OP_RECV
		*/
		case IOOperation::RECV:
		{
			// ��Ŷ ������ (�ӽ�) ���߿� �ٲ� ����
			int remain_data = io_size + sessionList[key].currentDataSize;
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
			sessionList[key].currentDataSize = remain_data;
			if (remain_data > 0)
				memcpy(curr_over_ex->dataBuffer, p, remain_data);
			DoRecv(sessionList[key]);

			break;
		}


		/**
		* OP_SEND
		*/
		case IOOperation::SEND:
		{
			// ���� OVER_EXP �����.
			delete curr_over_ex;
			std::println("Successfully sended.");
			break;
		}
		}
	}

	std::println("Worker Ended.");
}

void IOCP::DoRecv(Session& session) const
{

	DWORD recv_flag = 0;
	OverlappedEx& over_ex = session.overEx;

	ZeroMemory(over_ex.dataBuffer, sizeof(over_ex.dataBuffer));

	// ���� ���� ��ŭ recv�Ѵ�. 
	over_ex.wsabuf.len = BUFFER_SIZE - session.currentDataSize;
	over_ex.wsabuf.buf = over_ex.dataBuffer + session.currentDataSize;

	// �񵿱� Recv
	WSARecv(
		over_ex.clientSocket,
		&over_ex.wsabuf,
		1,
		0,
		&recv_flag,
		&over_ex.over,
		0);
}

void IOCP::DoSend(Session& session, void* packet)
{
	OverlappedEx* send_over_ex = new OverlappedEx{ reinterpret_cast<unsigned char*>(packet) };
	WSASend(session.clientSocket, &send_over_ex->wsabuf, 1, 0, 0, &send_over_ex->over, 0);
}

void IOCP::ProcessPacket(int key, char* p)
{
	std::println("Processing Packet.");
	packet::SCLogin packet{};
	DoSend(sessionList[key], reinterpret_cast<void*>(&packet));
}




IOCP::~IOCP()
{
	closesocket(listenSocket);
	WSACleanup();
}
