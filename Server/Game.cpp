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
	matchmakingQueue.push(id);

	float waiting_time = timeDist(dre);
	std::array<int, 3> client_ids{};
	bool loop{ false };
	bool match{ false };
	Timer pop_timer;
	int count{ 1 };
	do {
		loop = false;

		// 매치메이킹 큐에 세명이 찰 경우 3명을 뽑음
		if (matchmakingQueue.unsafe_size() >= 3) {
			for (auto& id : client_ids) {
				if (not matchmakingQueue.try_pop(id)) {
					loop = true;
					break;
				}
			}
			match = true;
		}
		
		// 실패하면 대기시간을 점점 더 늘려가면서 재시도
		if (loop) {
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
		while (not roomQueue.try_pop(roomId)) {
			std::this_thread::yield();
		}
		for (int i = 0; i < 3; ++i) {
			while (not playerQueue.try_pop(player_ids[i])) {
				std::this_thread::yield();
			}
			roomList[roomId].clientIdList[i] = client_ids[i];
			GET_SINGLE(IOCP)->SetClientIdInfo(client_ids[i], player_ids[i], roomId);
			playerList[player_ids[i]].x = posDist(dre);
			playerList[player_ids[i]].y = posDist(dre);
			playerList[player_ids[i]].clientId = client_ids[i];
		}

	}

}

void Game::SetPlayerPosition(const int player_id, Vec2f pos)
{
	playerList[player_id].pos = pos;
}

Vec2f Game::GetPlayerPosition(const int player_id) const
{

	return playerList[player_id].pos;
}

std::array<int, 3> Game::GetRoomClients(const int room_id)
{
	return roomList[room_id].clientIdList;
}

void Game::Init()
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		playerQueue.push(i);
	}

	for (int i = 0; i < MAX_ROOM; ++i) {
		roomQueue.push(i);
	}
}
