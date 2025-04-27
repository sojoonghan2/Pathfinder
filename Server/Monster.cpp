#include "pch.h"
#include "Monster.h"
#include "Room.h"


void Monster::Update(const float delta_time)
{
}


bool Monster::TrySetRunning(const bool running)
{
	bool expected = not running;
	return _isRunning.compare_exchange_strong(expected, running);
}

bool Monster::GetRunning() const
{
	return _isRunning.load();
}
