#include "pch.h"
#include "Object.h"


void Object::SetPos(const Vec2f& pos)
{
	_pos = pos;
}

void Object::SetPos(const float x, const float y)
{
	_pos.x = x;
	_pos.y = y;
}


void Object::Move(const float delta_time)
{
	_dir.Normalize();
	_pos.x += _dir.x * _speed * delta_time;
	_pos.y += _dir.y * _speed * delta_time;
}

bool Object::CheckCollision(const std::shared_ptr<Object>& other)
{
	if (nullptr == other) {
		return false;
	}
	if (nullptr == _collider || nullptr == other->GetCollider()) {
		return false;
	}

	return _collider->CheckCollision(other->GetCollider());
}
