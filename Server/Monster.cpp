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
