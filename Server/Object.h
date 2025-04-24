#pragma once

class Object
{
public:
	void Move(const Vec2f& pos);
	void Move(const float x, const float y);

	void MoveByDelta(const float x, const float y);

	// getter and setter
	Vec2f GetPos() const { return _pos; }

	Vec2f GetDir() const { return _dir; }
	void SetDir(const Vec2f& dir) { _dir = dir; }

private:
	Vec2f		_pos{};
	Vec2f		_dir{};
};

