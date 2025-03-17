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

	// 현재 사용 가능한 playerid의 큐
	concurrency::concurrent_queue<int> playerQueue;
	// 현재 사용 가능한 roomid의 큐
	concurrency::concurrent_queue<int> roomQueue;
	// 현재 매칭을 등록한 clientid의 큐
	concurrency::concurrent_queue<int> matchmakingQueue;

};

