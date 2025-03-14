#include "pch.h"
#include "Player.h"

#include "IOCP.h"


std::random_device rd;
std::default_random_engine dre{ rd() };
std::uniform_real_distribution<float> urd{ -24.f, 24.f };


class Timer {
private:
	using Clock = std::chrono::steady_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using MilliSeconds = std::chrono::duration<float, std::milli>; // 밀리초 단위로 변경

	TimePoint lastFrame;
	float deltaTimeMS; // 밀리초 단위의 델타타임

public:
	Timer() : lastFrame(Clock::now()), deltaTimeMS(0.0f) {}

	float getDeltaTimeMS() const { return deltaTimeMS; }
	float getDeltaTimeSeconds() const { return deltaTimeMS / 1000.0f; } // 초 단위 변환

	void updateDeltaTime() {
		TimePoint currentFrame = Clock::now();
		deltaTimeMS = std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
		lastFrame = currentFrame;
	}

	float PeekDeltaTime() const {
		TimePoint currentFrame = Clock::now();
		return std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
	}
};



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
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), IOCPHandle, 9999, 0);

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


	for (int i = 0; i < 1; ++i) {
		workers.emplace_back([this]() { Worker(); });
	}
	workers.emplace_back([this]() {TimerWorker(); });
	std::println("Created Thread.");

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

			sessionList[client_id].currentDataSize = 0;
			sessionList[client_id].clientSocket = acceptSocket;
			sessionList[client_id].overEx.clientSocket = acceptSocket;

			// IOCP 객체에 받아들인 클라이언트의 소켓을 연결
			auto ret = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(acceptSocket),
				IOCPHandle,
				client_id,
				0);

			// WSARecv 호출.
			DoRecv(sessionList[client_id]);

			// accept를 위한 새로운 소켓 생성
			acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

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
			std::println("AcceptEx Successed.");
			break;
		}

		/**
		* OP_RECV
		*/
		case IOOperation::RECV:
		{
			// 패킷 재조립 (임시) 나중에 바꿀 예정

			int remain_data = io_size + sessionList[key].currentDataSize;
			char* p = curr_over_ex->dataBuffer;

			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					bool ret = ProcessPacket(static_cast<int>(key), p);
					if (false == ret) {
						closesocket(sessionList[key].clientSocket);
						ZeroMemory(&sessionList[key], sizeof(Session));
						break;
					}
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
	Timer timer;
	while (true) {
		if (timer.PeekDeltaTime() > 50.f) {
			timer.updateDeltaTime();
			for (int i = 0; i < sessionCnt; ++i) {
				packet::SCMovePlayer packet{ i, players[i].x, players[i].y };
				DoBroadcast(i, &packet);
			}
		}
		std::this_thread::yield();
	}
}

void IOCP::DoRecv(Session& session) const
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


void IOCP::DoSend(Session& session, void* packet)
{
	OverlappedEx* send_over_ex = new OverlappedEx{ reinterpret_cast<unsigned char*>(packet) };
	WSASend(session.clientSocket, &send_over_ex->wsabuf, 1, 0, 0, &send_over_ex->over, 0);
}

bool IOCP::ProcessPacket(int key, char* p)
{
	packet::Header* header = reinterpret_cast<packet::Header*>(p);
	switch (header->type)
	{
	case packet::Type::CS_LOGIN:
	{

		std::println("CS_LOGIN Client {}", key);

		packet::SCLogin sc_login{ key };
		DoSend(sessionList[key], &sc_login);

		for (int i = 0; i < sessionCnt - 1; ++i) {
			packet::SCMovePlayer sc_move_player{ i, players[i].x, players[i].y };
			DoSend(sessionList[key], &sc_move_player);
		}

		players[key].x = urd(dre);
		players[key].y = urd(dre);
		packet::SCMovePlayer sc_move_player{ key, players[key].x, players[key].y };
		DoBroadcast(&sc_move_player);
	}
	break;

	case packet::Type::CS_MOVE_PLAYER:
	{
		packet::CSMovePlayer* packet = reinterpret_cast<packet::CSMovePlayer*>(p);

		players[key].x = packet->x;
		players[key].y = packet->y;
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
	for (int i = 0; i < sessionCnt; ++i) {
		DoSend(sessionList[i], packet);
	}
}

void IOCP::DoBroadcast(int key, void* packet)
{
	for (int i = 0; i < sessionCnt; ++i) {
		if (key == i) continue;
		DoSend(sessionList[i], packet);
	}
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
