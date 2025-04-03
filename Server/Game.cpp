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
	// Ŭ���̾�Ʈ�� ��ġ����ŷ ��Ŷ�� �����Ͽ��� ���
	// Ŭ���̾�Ʈ ���̵� �켱���� ť�� �ְ� ������ �˻�
	// ����� 3�̻��̸� Ŭ���̾�Ʈ 3���� �̾Ƴ�.
	
	// ���� Ŭ���̾�Ʈ 3���� �̴µ� �����Ѵٸ�
	// ���� ���� Ŭ���̾�Ʈ id�� �켱������ ������ �ٽ� ��ġ����ŷ ť�� ����
	
	// ��ȣ 3���� �̴µ� �����Ѵٸ� 
	// �÷��̾� ��ȣ�� 3�� �÷��̾� ť���� ����.
	// ���� ��ȣ 3���� ���� Ŭ���̾�Ʈ�� �ο�
	// �� ��ȣ ť���� ��ȣ�� �ϳ��� ������
	// �� ��ȣ�� �濡 �÷��̾� ���̵� ����.
	matchmakingQueue.push(id);

	float waiting_time = timeDist(dre);
	std::array<int, 3> client_ids{};
	bool loop{ false };
	bool match{ false };
	Timer pop_timer;
	int count{ 1 };
	do {
		loop = false;

		// ��ġ����ŷ ť�� ������ �� ��� 3���� ����
		if (matchmakingQueue.unsafe_size() >= 3) {
			for (auto& id : client_ids) {
				if (not matchmakingQueue.try_pop(id)) {
					loop = true;
					break;
				}
			}
			match = true;
		}
		
		// �����ϸ� ���ð��� ���� �� �÷����鼭 ��õ�
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
