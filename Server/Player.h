#pragma once


struct Player
{
public:
	void Move(const Vec2f& pos);
	void Move(const float x, const float y);

	bool GetRunning() const { return _isRunning.load(); }
	bool TrySetRunning(const bool running);

	Vec2f GetPos() const { return _pos; }

private:
	Vec2f	_pos{};
	float&	_x{ _pos.x };
	float&	_y{ _pos.y };

	std::atomic_bool _isRunning{ false };
};