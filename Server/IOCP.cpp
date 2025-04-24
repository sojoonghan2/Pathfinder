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
	// 윈도우 초기화
	WSADATA wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != ret) {
		util::DisplayError();
		return false;
	}

	// listen socket 소켓 만들기
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	if (INVALID_SOCKET == _listenSocket) {
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

	// IOCP 핸들 생성
	_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// 생성한 IOCP 핸들을 listen socket에 연결
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_listenSocket), _IOCPHandle, 99999, 0);

	std::println("IOCP Init Successed.");

	return true;
}

bool IOCP::Start()
{
	// 클라이언트 accept socket 생성
	_acceptSocket = WSASocket(
		AF_INET,
		SOCK_STREAM,
		0,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED);
	int flag = 1;
	setsockopt(_acceptSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

	// accpet를 위한 OverlappedEx 생성
	OverlappedEx accept_over_ex;
	accept_over_ex.operation = IOOperation::ACCEPT;
	accept_over_ex.clientSocket = _acceptSocket;

	int addr_size = sizeof(SOCKADDR_IN);
	DWORD bytes_received{};


	// listen_socket에 accept_socket 비동기 Accept 등록
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

	// 비동기 IO 작업 완료 확인 스레드 생성
	// worker_thread 따로 멤버변수로 뺴기

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
		
		// 예상 volatile 필요한 위치
		int key = static_cast<int>(ULkey);

		if (FALSE == ret) {
			// TODO: ERROR
			closesocket(_clientInfoHash[key].clientSocket);
			_clientInfoHash[key].ioState = IOState::DISCONNECT;
			// Send 일경우 보낸 curr_ex는 제거
			if (curr_over_ex->operation == IOOperation::SEND) {
				delete curr_over_ex;
			}
		}


		// 완료된 작업의 OverlappedEx 정보를 읽는다.
		// 어떤 operation으로 완료되었는지 확인. 
		switch (curr_over_ex->operation) {

			/**G
			* ACCEPT: AcceptEx() 작업 완료
			*/
		case IOOperation::ACCEPT:
		{
			int client_id = _sessionCnt++;
			_clientInfoHash.insert(std::make_pair(client_id, ClientInfo{}));
			_clientInfoHash[client_id].currentDataSize = 0;
			_clientInfoHash[client_id].clientSocket = _acceptSocket;
			_clientInfoHash[client_id].overEx.clientSocket = _acceptSocket;


			// IOCP 객체에 받아들인 클라이언트의 소켓을 연결
			auto ret = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(_acceptSocket),
				_IOCPHandle,
				client_id,
				0);

			// WSARecv 호출.
			DoRecv(_clientInfoHash[client_id]);

			// accept를 위한 새로운 소켓 생성
			_acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			int flag = 1;
			setsockopt(_acceptSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

			// Accept로 overlapped 설정 
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
	Timer update_timer;
	Timer send_timer;
	while (true) {

		update_timer.updateDeltaTime();
		auto delta = update_timer.getDeltaTimeSeconds();
		GET_SINGLE(Game)->Update(delta);


		// 몬스터의 위치를 플레이어한테 전송
		if (send_timer.PeekDeltaTime() > MOVE_PACKET_TIME_MS) {
			send_timer.updateDeltaTime();



			// 현재 방이 활성화 되어있는지 확인
			// 일단 킵
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

				// 패킷 생성
				// 일단 임시로 MAX_PLAYER를 더한 아이디를 준다.
				// TODO: 오브젝트를 한 자료구조에(shared_ptr) 저장하게 되면 id만을 보낼 것.
				packet::SCMoveMonster sc_monster_move{
					MAX_PLAYER + i,
					monster_pos.x,
					monster_pos.y,
					monster_dir.x,
					monster_dir.y
				};
				// 방에 있는 플레이어에게 전송
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

void IOCP::DoSend(int client_id, void* packet)
{
	DoSend(_clientInfoHash[client_id], packet);
}

void IOCP::ProcessPacket(int key, char* p)
{

	// IOCP에서 처리
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
		// 클라이언트에 매치메이킹 패킷이 도착하였을 경우
		// 클라이언트 아이디를 큐에 넣고 사이즈 검사
		// 사이즈가 3이상이면 클라이언트 3개를 뽑아냄.

		// 만약 클라이언트 3개를 뽑는데 실패한다면
		// 여태 뽑은 클라이언트 id를 우선순위를 높혀서 다시 매치메이킹 큐에 저장

		// 번호 3개를 뽑는데 성공한다면 
		// 플레이어 번호를 3개 플레이어 큐에서 꺼냄.
		// 꺼낸 번호 3개를 각각 클라이언트에 부여
		// 방 번호 큐에서 번호를 하나를 꺼내와
		// 그 번호의 방에 플레이어 아이디를 저장.

		// TODO:
		// 최적화 할 것: 매치매이킹 패킷이 여러개 들어오면 짤라야 한다.
		// atomic한 STATE 변수가 필요할 예정
		
		// 메치메이킹 이럴거면 그냥 락쓰자.. 그게 낫겠다
	

		// 일단 아이디를 넣는다.
		_matchmakingQueue.push(key);

		// 3 미만이면 그냥 나가기
		if (_matchmakingQueue.unsafe_size() < 3) {
			break;
		}


		// 현재 클라이언트가 3 이상이면 다음과 같은 매치메이킹을 실행
		// 매칭 실패시 대기할 시간
		float waiting_time = timeDist(dre);

		// 큐에서 뽑아낼 3명의 클라이언트 아이디를 저장할 배열
		std::array<int, 3> client_ids{};
		client_ids.fill(-1);

		bool loop{ false };
		bool match{ false };
		Timer pop_timer;
		int count{ 1 };

		// 매치메이킹 알고리즘
		do {
			loop = false;

			// 매치메이킹 큐에 세명이 찰 경우 3명을 뽑음
			if (_matchmakingQueue.unsafe_size() >= 3) {
				for (auto& id : client_ids) {
					if (not _matchmakingQueue.try_pop(id)) {
						loop = true;
						break;
					}
				}
				match = true;
			}

			// 실패하면 다시 넣고 대기시간을 점점 더 늘려가면서 재시도
			if (loop) {

				// 다시 넣음
				for (auto& id : client_ids) {
					if (-1 != id) {
						_matchmakingQueue.push(id);
					}
				}

				// 재시도
				pop_timer.updateDeltaTime();
				while (pop_timer.PeekDeltaTime() < waiting_time) {
					std::this_thread::yield();
				}
				waiting_time += timeDist(dre) * count++;
			}
		} while (loop);



		// -- 클라이언트 세명 매칭 완료 --

		// 방 번호를 얻어옴
		int room_id{ -1 };
		for (int i{}; i < MAX_ROOM; ++i) {
			if (not _roomInfoList[i].GetRunning()) {
				if (_roomInfoList[i].CASRunning(false, true)) {
					// 방 번호 얻기 성공
					room_id = i;
					break;
				}
			}
			
		}

		// 방 번호를 얻어오는 데 실패함.
		if (-1 == room_id) {
			// TODO: 여기 부분을 최적화
			// 일단 임시로 다시 넣고 종료.
			for (auto& id : client_ids) {
				if (-1 != id) {
					_matchmakingQueue.push(id);
				}
			}
			break;
		}

		// 플레이어 아이디는 방 번호에 따라 고정.
		std::array<int, 3> player_ids{
			room_id * 3,
			room_id * 3 + 1,
			room_id * 3 + 2
		};

		// -- 방 번호와 플레이어 번호를 얻어왔음. -- 
		// 이제 게임을 시작히기 위해
		// Room, RoomInfo, Player에다가 정보를 저장

		// 게임 쪽 초기화. 방, 플레이어, 몬스터의 실제 정보를 초기화해준다.
		GET_SINGLE(Game)->InitRoom(room_id);
		
		// roominfo에 로딩 완료된 플레이어 0명으로 설정
		_roomInfoList[room_id].SetLoadingCount(0);

		for (int i = 0; i < 3; ++i) {
			// RoomInfo에 정보를 넣음
			_roomInfoList[room_id].InsertClient(i, client_ids[i]);

			// ClientInfoHash에 정보를 넣음
			_clientInfoHash[client_ids[i]].clientIdInfo.roomId = room_id;
			_clientInfoHash[client_ids[i]].clientIdInfo.playerId = player_ids[i];
			_clientInfoHash[client_ids[i]].ioState = IOState::INGAME;
		}

		// SC_MATCHMAKING 매칭 완료 알림 및 로드할 방의 타입을 줌.
		auto room_type{ GET_SINGLE(Game)->GetRoom(room_id).GetRoomType() };
		
		// 클라이언트 로딩 시작
		for (int i = 0; i < 3; ++i) {
			packet::SCMatchmaking matchmaking_packet{ client_ids[i], room_type };
			DoSend(client_ids[i], &matchmaking_packet);
		}


	
	}
	break;

	case packet::Type::CS_LOAD_COMPLETE:
	{
		// TODO:
		// 예외 처리 고고

		// 대기 완료 플레이어 수를 조정
		auto room_id{ _clientInfoHash[key].clientIdInfo.roomId };
		
		// 혹시 모를 예외처리
		if (false == _roomInfoList[room_id].GetRunning()) {
			Disconnect(key);
		}

		// 플레이어 로딩 현황 갱신
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

		// 아직 로딩이 안됐음
		if (not load_complete) {
			break;
		}

		// 로딩 완료
		// 세명의 플레이어가 다 로딩이 완료되었다면
		// 방의 상태를 RUNNING으로 하고 ( room update에 쓰이는 변수 )
		// 게임 시작 패킷을 보낼 것.

		// 로딩 카운트 초기화.
		_roomInfoList[room_id].SetLoadingCount(0);

		// 클라이언트 아이디와 플레이어 아이디를 가져온다.
		auto client_ids{ _roomInfoList[room_id].GetClientIdList() };
		std::array<int, 3> player_ids{
			room_id * 3,
			room_id * 3 + 1,
			room_id * 3 + 2
		};


		// 방에 있는 플레이어들에게 위치를 보냄
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

		// 게임 시작. 방 상태를 Running으로.
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

		// 받은 위치를 저장
		auto& player = GET_SINGLE(Game)->GetPlayer(id_info.playerId);
		player.SetDir(Vec2f{ packet->dirX, packet->dirY });
		player.Move(packet->x, packet->y);

		auto pos = player.GetPos();
		auto dir = player.GetDir();

		// 다른 플레이어에게 전송
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

		// todo: 패킷 에러시 클라이언트 종료
		_clientInfoHash[key].ioState = IOState::DISCONNECT;
		closesocket(_clientInfoHash[key].clientSocket);

		return;
	}
	}


}

void IOCP::DoBroadcast(void* packet)
{
	// todo: 수정
	for (int i = 0; i < _sessionCnt; ++i) {
		DoSend(_clientInfoHash[i], packet);
	}
}

void IOCP::DoBroadcast(int key, void* packet)
{
	// todo: 수정
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
	// 나중에 만들 atomic_shared_ptr로 자동으로 지울 예정.
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
