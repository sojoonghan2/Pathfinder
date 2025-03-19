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

	// 현재 사용 가능한 playerid의 큐
	concurrency::concurrent_queue<int> playerQueue;
	// 현재 사용 가능한 roomid의 큐
	concurrency::concurrent_queue<int> roomQueue;
	// 현재 매칭을 등록한 clientid의 큐
	concurrency::concurrent_queue<int> matchmakingQueue;

};

