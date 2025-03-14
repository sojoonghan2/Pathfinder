#pragma once
#include "Player.h"

// temp
struct Room
{
	void lock() { roomMutex.lock(); }
	void unlock() { roomMutex.unlock(); }

	std::mutex roomMutex;
	std::unordered_map<int, Vec2f> players;
};

class Game
{
	DECLARE_SINGLE(Game)


public:
	bool EnterRoom(int id);
	void SetPosition(const int id, Vec2f pos);
	void SetRandomPosition(const int id);
	std::vector<int> GetRoomPlayers(const int id);

private:
	std::array<Room, ROOM_COUNT> rooms;
	concurrency::concurrent_unordered_map<int, Room*> roomHash;
	
};

