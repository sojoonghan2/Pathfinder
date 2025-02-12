#include <WS2tcpip.h>
#include <MSWSock.h>

#include <thread>
#include <vector>
#include <array>
#include <print>
#include <iostream>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

HANDLE		IOCPHandle = INVALID_HANDLE_VALUE;
int SessionCnt;

enum class IOOperation
{
	ACCEPT,
	RECV,
	SEND
};

//enum class IOState
//{ 
//	FREE,
//	ALLOC,
//	INGAME
//};



struct OverlappedEx
{
	WSAOVERLAPPED	overlapped; // overlapped IO 구조체
	SOCKET			client_socket;		// 클라이언트 소켓
	WSABUF			wsabuf;		// 작업 버퍼
	char			data_buffer[BUF_SIZE]; // 데이터 버퍼
	IOOperation		operation;	// 동작 종류

	// recv 전용 생성자. 
	OverlappedEx()
	{
		wsabuf.len = BUF_SIZE;
		wsabuf.buf = data_buffer;
		operation = IOOperation::RECV;
		ZeroMemory(&overlapped, sizeof(overlapped));
	}
};

struct Session {

	// RECV, SEND에 사용할 Overlapped 변수
	OverlappedEx	overlapped_ex;
	SOCKET			socket;

	Session() :
		socket{ INVALID_SOCKET }
	{
		ZeroMemory(&overlapped_ex, sizeof(overlapped_ex));
	}

	void do_recv()
	{
		DWORD recv_flag = 0;
		WSARecv(socket, &overlapped_ex.wsabuf, 1, 0, &recv_flag, &overlapped_ex.overlapped, 0);
	}

};

std::array<Session, 10> SessionList;

void worker(SOCKET listen_socket)
{
	std::println("Hello worker!");
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		auto ret = GetQueuedCompletionStatus(IOCPHandle, &num_bytes, &key, &over, INFINITE);
		OverlappedEx* ex_over = reinterpret_cast<OverlappedEx*>(over);

		if (FALSE == ret) {
			std::println("Client Error.\n {}", GetLastError());
			exit(-1);
		}

		// 완료된 작업의 OverlappedEx 정보를 읽는다.
		// 어떤 operation으로 완료되었는지 확인. 
		switch (ex_over->operation) {

			/**
			* ACCEPT: AcceptEx() 작업 완료
			*/
		case IOOperation::ACCEPT:
		{
			// 세션 컨테이너에 소켓 정보 저장
			int client_id = SessionCnt++;
			std::println("New Client {} Accepted.", client_id);
			SOCKET current_socket = ex_over->client_socket;
			SessionList[client_id].socket = current_socket;

			// IOCP 객체에 받아들인 클라이언트의 소켓을 연결
			auto ret = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(current_socket),
				IOCPHandle,
				client_id,
				0);

			// WSARecv 호출.

			// 다시 accept를 위한 새로운 소켓 생성
			current_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

			// overlapped 
			ZeroMemory(&ex_over->overlapped, sizeof(ex_over->overlapped));
			ex_over->client_socket = current_socket;
			ex_over->operation = IOOperation::ACCEPT;
			int addr_size = sizeof(SOCKADDR_IN);

			AcceptEx(
				listen_socket,
				current_socket,
				ex_over->data_buffer,
				0,
				addr_size + 16,
				addr_size + 16,
				0,
				&ex_over->overlapped
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


int main()
{
	// 윈도우 초기화
	WSADATA wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != ret) {
		std::println("Error: WSAStartup() Failed.");
		return -1;
	}
	std::println("WSAStartup() Successed.");

	// 소켓 만들기
	SOCKET listen_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == listen_socket) {
		std::println("Error: Failed to create server socket.");
		return -1;
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

	// IOCP 핸들 생성
	IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// 생성한 IOCP 핸들을 listen socket에 연결
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listen_socket), IOCPHandle, 9999, 0);

	// 클라이언트 accept socket 생성
	SOCKET accept_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	// accpet를 위한 OverlappedEX 생성
	OverlappedEx accept_over;
	accept_over.operation = IOOperation::ACCEPT;
	accept_over.client_socket = accept_socket;
	
	int addr_size = sizeof(SOCKADDR_IN);

	// listen_socket에 accept_socket 비동기 Accept 등록
	AcceptEx(
		listen_socket,
		accept_socket,
		accept_over.data_buffer,
		0,
		addr_size + 16,
		addr_size + 16,
		0,
		&accept_over.overlapped
	);

	std::println("AcceptEx Successed.");


	// unsigned int num_thread{ std::thread::hardware_concurrency() };
	
	// 비동기 IO 작업 완료 확인 스레드 생성
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