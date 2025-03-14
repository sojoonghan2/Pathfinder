#include "pch.h"
#include "Game.h"

bool Game::EnterRoom(int id)
{
	for (auto& room : rooms) {
		room.lock();
		auto number = room.players.size();
		if (number >= 3) {
			room.unlock();
			continue;
		}
		room.players
		room.unlock();
		roomHash.insert({ id, &room });
	}
}

void Game::SetPosition(const int id, Vec2f pos)
{

}

void Game::SetRandomPosition(const int id)
{
	
}

std::vector<int> Game::GetRoomPlayers(const int id)
{
	roomHash.find(id)
}
