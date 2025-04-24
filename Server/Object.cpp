#include "pch.h"
#include "Object.h"


void Object::Move(const Vec2f& pos)
{
	_pos = pos;
}

void Object::Move(const float x, const float y)
{
	_pos.x = x;
	_pos.y = y;
}

void Object::MoveByDelta(const float x, const float y)
{
	Move(_pos.x + x, _pos.y + y);
}
