#include "pch.h"
#include "Monster.h"
#include "Room.h"


void Monster::Move(const Vec2f& pos)
{
	_pos = pos;
}

void Monster::Move(const float x, const float y)
{
	_x = x;
	_y = y;
}

void Monster::Update(const float delta_time)
{
	// 위치 업데이트
	// milliseconds 단위이므로 바꾼다.
	_x = _pos.x + _dir.x * _speed * delta_time;
	_y = _pos.y + _dir.y * _speed * delta_time;
}

void Monster::NormalizeAndSetDir(const Vec2f& dir)
{
	// normalize
	float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
	if (length > 0.f) {
		_dir.x = dir.x / length;
		_dir.y = dir.y / length;
	}
	else {
		_dir.x = 0.f;
		_dir.y = 0.f;
	}
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
