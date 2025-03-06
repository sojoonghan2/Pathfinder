#pragma once

#define NETWORK_START	namespace network {
#define NETWORK_END		}	


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
