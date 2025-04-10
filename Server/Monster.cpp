#include "pch.h"
#include "Monster.h"

void Monster::Move(const Vec2f& pos)
{
	_pos = pos;
}

void Monster::Move(const float x, const float y)
{
	_x = x;
	_y = y;
}

void Monster::Update()
{
	// 가장 가까운 플레이어의 위치를 찾아서
	// 그 방향으로 이동하는 로직을 구현해야 함.
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
