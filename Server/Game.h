#pragma once
#include "Player.h"

class Game
{
	DECLARE_SINGLE(Game)

private:
	~Game() {}


public:
	void RegisterClient(int id);
	void SetPlayerPosition(const int player_id, Vec2f pos);
	Vec2f GetPlayerPosition(const int player_id) const;
	std::array<int, 3> GetRoomClients(const int room_id);
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

