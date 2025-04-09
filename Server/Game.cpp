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

	// �ϴ� ���̵� �ִ´�.
	_matchmakingQueue.push(id);

	// ��Ī ���н� ����� �ð�
	float waiting_time = timeDist(dre);

	// ť���� �̾Ƴ� 3���� Ŭ���̾�Ʈ ���̵� ������ �迭
	std::array<int, 3> client_ids{};
	client_ids.fill(-1);

	bool loop{ false };
	bool match{ false };
	Timer pop_timer;
	int count{ 1 };
	do {
		loop = false;

		// ��ġ����ŷ ť�� ������ �� ��� 3���� ����
		if (_matchmakingQueue.unsafe_size() >= 3) {
			for (auto& id : client_ids) {
				if (not _matchmakingQueue.try_pop(id)) {
					loop = true;
					break;
				}
			}
			match = true;
		}
		
		// �����ϸ� �ٽ� �ְ� ���ð��� ���� �� �÷����鼭 ��õ�
		if (loop) {

			// �ٽ� ����
			for (auto& id : client_ids) {
				if (-1 != id) {
					_matchmakingQueue.push(id);
				}
			}

			// ��õ�
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

		// ������ ���� ã��
		for (int i{}; i < MAX_ROOM; ++i) {
			if (not _roomList[i].GetRunning()) {
				if (_roomList[i].TrySetRunning(true)) {
					roomId = i;
					break;
				}
			}
		}

		// ���� �� ���ִ� ������.
		if (roomId == -1) {

			// �ϴ� �ӽ÷� �ٽ� �ְ� ����.
			// ����ȭ:
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



			// �÷��̾ �� ���ִ� ������.
			if (player_id == -1) {
				// �ϴ� �ӽ÷� �ٽ� �ְ� ����.
				// ����ȭ: 
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
