#include "pch.h"

HANDLE		IOCPHandle = INVALID_HANDLE_VALUE;
int SessionCnt;
std::array<Session, 10> SessionList;

void worker(SOCKET listen_socket)
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


		// �Ϸ�� �۾��� OverlappedEx ������ �д´�.
		// � operation���� �Ϸ�Ǿ����� Ȯ��. 
		switch (curr_over_ex->operation) {

			/**
			* ACCEPT: AcceptEx() �۾� �Ϸ�
			*/
		case IOOperation::ACCEPT:
		{
			// ���� �����̳ʿ� ���� ���� ����
			int client_id = SessionCnt++;
			std::println("New Client {} Accepted.", client_id);
			SOCKET curr_socket = curr_over_ex->client_socket;
			SessionList[client_id].socket = curr_socket;

			// IOCP ��ü�� �޾Ƶ��� Ŭ���̾�Ʈ�� ������ ����
			auto ret = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(curr_socket),
				IOCPHandle,
				client_id,
				0);

			// WSARecv ȣ��.


			// accept�� ���� ���ο� ���� ����
			curr_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

			// Accept�� overlapped ���� 
			ZeroMemory(&curr_over_ex->overlapped, sizeof(curr_over_ex->overlapped));
			curr_over_ex->client_socket = curr_socket;
			curr_over_ex->operation = IOOperation::ACCEPT;
			int addr_size = sizeof(SOCKADDR_IN);
			DWORD bytes_received{};

			AcceptEx(
				listen_socket,
				curr_socket,
				curr_over_ex->data_buffer,
				0,
				addr_size + 16,
				addr_size + 16,
				&bytes_received,
				&curr_over_ex->overlapped
			);
			std::println("AcceptEx Successed.");
			break;
		}

		/**
		* OP_RECV
		*/
		case IOOperation::RECV:
		{
			// ��Ŷ ������
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
			// ���� OVER_EXP �����.
			// delete ex_over;
			break;
		}
		}
	}

}


int main()
{
	// ������ �ʱ�ȭ
	WSADATA wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != ret) {
		std::println("Error: WSAStartup() Failed.");
		return -1;
	}
	std::println("WSAStartup() Successed.");

	// ���� �����
	SOCKET listen_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == listen_socket) {
		std::println("Error: Failed to create server socket.");
		return -1;
	}
	std::println("Created listen socket.");

	// ���� �ּ� ����
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	// bind
	ret = bind(listen_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (0 != ret) {
		std::println("Error: Failed to bind socket.");
		return -1;
	}
	std::println("Bind Successed.");

	// listen
	ret = listen(listen_socket, SOMAXCONN);
	if (0 != ret) {
		std::println("Error: Failed to listen.");
		return -1;
	}
	std::println("Listen Successed.");

	// IOCP �ڵ� ����
	IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// ������ IOCP �ڵ��� listen socket�� ����
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listen_socket), IOCPHandle, 9999, 0);

	// Ŭ���̾�Ʈ accept socket ����
	SOCKET accept_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	// accpet�� ���� OverlappedEX ����
	OverlappedEx accept_over;
	accept_over.operation = IOOperation::ACCEPT;
	accept_over.client_socket = accept_socket;
	
	int addr_size = sizeof(SOCKADDR_IN);
	DWORD bytes_received{};

	// listen_socket�� accept_socket �񵿱� Accept ���
	AcceptEx(
		listen_socket,
		accept_socket,
		accept_over.data_buffer,
		0,
		addr_size + 16,
		addr_size + 16,
		&bytes_received,
		&accept_over.overlapped
	);

	std::println("AcceptEx Successed.");


	// unsigned int num_thread{ std::thread::hardware_concurrency() };
	
	// �񵿱� IO �۾� �Ϸ� Ȯ�� ������ ����
	std::vector<std::thread> worker_thread;
	for (int i = 0; i < 1; ++i) {
		worker_thread.emplace_back(worker, listen_socket);
	}
	std::println("Created Worker Thread.");


	for (auto& thread : worker_thread) {
		thread.join();
	}

	closesocket(listen_socket);
	WSACleanup();
}