#pragma once
#include "Player.h"

class Game
{
	DECLARE_SINGLE(Game)

private:
	~Game() {}


public:
	bool RegisterClient(int id);
	void SetPosition(const int id, Vec2f pos);
	std::array<int, 3> GetRoomPlayers(const int id);
	void Init();

private:
	std::array<Player, PLAYER_COUNT> playerList{};
	std::array<Room, ROOM_COUNT>	roomList{};

	// ���� ��� ������ playerid�� ť
	concurrency::concurrent_queue<int> playerQueue;
	// ���� ��� ������ roomid�� ť
	concurrency::concurrent_queue<int> roomQueue;
	// ���� ��Ī�� ����� clientid�� ť
	concurrency::concurrent_queue<int> matchmakingQueue;

};

