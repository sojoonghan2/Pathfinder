#pragma once

#define PACKET_START namespace packet {
#define PACKET_END }


PACKET_START
enum class Type : unsigned char
{
	NONE,
	CS_LOGIN,
	SC_LOGIN,
	SC_MOVE_PLAYER,
	CS_MOVE_PLAYER,
};

#pragma pack(push, 1)
struct Header
{
	unsigned short	size{ sizeof(Header) };
	Type			type{ Type::NONE };
};

// No Param
struct CSLogin : Header
{
	CSLogin()
	{
		size = sizeof(CSLogin);
		type = Type::CS_LOGIN;
	}
};

// No Param
struct SCLogin : Header
{
	SCLogin()
	{
		size = sizeof(SCLogin);
		type = Type::SC_LOGIN;
	}
};

// Param:
//	int playerId
//	float x
//	float y
struct SCMovePlayer : Header
{
	int playerId{ 0 };
	float x{ 0.f };
	float y{ 0.f };

	SCMovePlayer()
	{
		size = sizeof(SCMovePlayer);
		type = Type::SC_MOVE_PLAYER;
	}
};

// Param:
//	int playerId
//	float x
//	float y
struct CSMovePlayer : Header
{
	int playerId{ 0 };
	float x{ 0.f };
	float y{ 0.f };

	CSMovePlayer()
	{
		size = sizeof(CSMovePlayer);
		type = Type::CS_MOVE_PLAYER;
	}
};

#pragma pack(pop)
PACKET_END
