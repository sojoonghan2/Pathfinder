#include "pch.h"
#include "Player.h"

void Player::Move(const Vec2f& pos)
{
	_pos = pos;
}

void Player::Move(const float x, const float y)
{
	_x = x;
	_y = y;
}

bool Player::TrySetRunning(const bool running)
{
	bool expected = !running;
	return _isRunning.compare_exchange_strong(expected, running);
}
