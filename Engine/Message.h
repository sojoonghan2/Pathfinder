#pragma once

enum class MsgType
{
	NONE = 0,
	MOVE,
	START_GAME,
};

struct Msg
{
	MsgType type{ MsgType::NONE };
	
	Msg() = default;
	Msg(const MsgType type) : type{ type }
	{}
};

struct MsgMove : Msg
{
	float x{};
	float y{};
	float dirX{};
	float dirY{};

	MsgMove() = default;
	MsgMove(const float x, const float y,
		const float dirX, const float dirY) :
		Msg{MsgType::MOVE},
		x{x}, y{y},
		dirX{dirX}, dirY{dirY}
	{}
};

struct MsgStartGame : Msg
{
	MsgStartGame() :
		Msg{ MsgType::START_GAME }
	{}
};
