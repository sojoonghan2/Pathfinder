#pragma once
struct Monster
{
public:
	void Move(const Vec2f& pos);
	void Move(const float x, const float y);

private:
	Vec2f _pos{};
	float& _x{ _pos.x };
	float& _y{ _pos.y };
};

