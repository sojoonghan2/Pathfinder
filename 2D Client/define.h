#pragma once

constexpr int PORT_NUMBER = 4000;
constexpr int BUFFER_SIZE = 200;

constexpr const char* SERVER_IP = "127.0.0.1";


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
