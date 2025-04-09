#include "pch.h"
#include "Game.h"
#include "IOCP.h"
#include "Timer.h"

std::random_device rd;
std::default_random_engine dre{ rd() };
std::uniform_real_distribution<float> timeDist{ 2.f, 4.f };
std::uniform_real_distribution<float> posDist{ -24.f, 24.f };

void Game::RegisterClient(int id)
{
	// 클라이언트에 매치메이킹 패킷이 도착하였을 경우
	// 클라이언트 아이디를 우선순위 큐에 넣고 사이즈 검사
	// 사이즈가 3이상이면 클라이언트 3개를 뽑아냄.
	
	// 만약 클라이언트 3개를 뽑는데 실패한다면
	// 여태 뽑은 클라이언트 id를 우선순위를 높혀서 다시 매치메이킹 큐에 저장
	
	// 번호 3개를 뽑는데 성공한다면 
	// 플레이어 번호를 3개 플레이어 큐에서 꺼냄.
	// 꺼낸 번호 3개를 각각 클라이언트에 부여
	// 방 번호 큐에서 번호를 하나를 꺼내와
	// 그 번호의 방에 플레이어 아이디를 저장.

	// 일단 아이디를 넣는다.
	_matchmakingQueue.push(id);

	// 매칭 실패시 대기할 시간
	float waiting_time = timeDist(dre);

	// 큐에서 뽑아낼 3명의 클라이언트 아이디를 저장할 배열
	std::array<int, 3> client_ids{};
	client_ids.fill(-1);

	bool loop{ false };
	bool match{ false };
	Timer pop_timer;
	int count{ 1 };
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


	if (match) {
		int roomId = -1;
		std::array<int, 3> player_ids{};

		// 가능한 방을 찾음
		for (int i{}; i < MAX_ROOM; ++i) {
			if (not _roomList[i].GetRunning()) {
				if (_roomList[i].TrySetRunning(true)) {
					roomId = i;
					break;
				}
			}
		}

		// 방이 꽉 차있는 상태임.
		if (roomId == -1) {

			// 일단 임시로 다시 넣고 종료.
			// 최적화:
			for (auto& id : client_ids) {
				if (-1 != id) {
					_matchmakingQueue.push(id);
				}
			}
			return;


		}


		for (int i{}; i < 3; ++i) {

			int player_id{ -1 };
			for (int ii{}; ii < MAX_PLAYER; ++ii) {
				if (not _playerList[ii].GetRunning()) {
					if (_playerList[ii].TrySetRunning(true)) {
						player_id = ii;
						break;
					}
				}
			}



			// 플레이어가 꽉 차있는 상태임.
			if (player_id == -1) {
				// 일단 임시로 다시 넣고 종료.
				// 최적화: 
				for (auto& id : client_ids) {
					if (-1 != id) {
						_matchmakingQueue.push(id);
					}
				}
				return;
			}

			_roomList[roomId]._clientIdList[i] = client_ids[i];
			GET_SINGLE(IOCP)->SetClientIdInfo(client_ids[i], player_ids[i], roomId);
			_playerList[player_ids[i]].Move(posDist(dre), posDist(dre));
			_playerList[player_ids[i]].SetClientId(client_ids[i]);
		}

	}

}


void Game::ProcessPacket(const int key, void* packet)
{
	packet::Header* header = reinterpret_cast<packet::Header*>(packet);

	switch (header->type)
	{
	case packet::Type::SC_LOGIN:
	{

	}
	break;
	case packet::Type::SC_MATCHMAKING:
	{
	}
	break;
	case packet::Type::SC_MOVE_PLAYER:
	{
	}
	break;

	default:
		break;
	}
}

void Game::Init()
{
	
}
