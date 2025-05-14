#pragma once

enum class MsgType
{
	NONE = 0,
	MOVE,
	START_GAME,
	REGISTER,
	FIRE_GUN,
	SET_OBJECT_HP,
};

// 인자가 필요 없는 것은 그냥 Msg로 처리
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
		Msg{ MsgType::MOVE },
		x{x}, y{y},
		dirX{dirX}, dirY{dirY}
	{}
};


struct MsgSetObjectHp : Msg
{
	float hp{ -1.f };
	float maxHp{ -1.f };
	bool isAttacker{ false };

	MsgSetObjectHp() = default;
	MsgSetObjectHp(const float hp, const float maxHp, const bool isAttacker = false) :
		Msg{ MsgType::SET_OBJECT_HP },
		hp{ hp },
		maxHp{ maxHp },
		isAttacker{ isAttacker }
	{}

};