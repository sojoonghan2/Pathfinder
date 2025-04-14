#pragma once

struct Vec2f
{
	float x{0.f};
	float y{0.f};

	Vec2f() = default;
	Vec2f(const float x, const float y) :
		x{ x },
		y{ y }
	{}

	Vec2f operator-(const Vec2f& other)
	{
		return Vec2f{ x - other.x, y - other.y };
	}

	Vec2f operator+(const Vec2f& other)
	{
		return Vec2f{ x + other.x, y + other.y };
	}
};
