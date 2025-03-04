#pragma once

struct Vec2f
{
	float x{};
	float y{};

	Vec2f() = default;
	Vec2f(const float x, const float y) :
		x{ x },
		y{ y }
	{}
};
