#pragma once

#define NETWORK_START	namespace network {
#define NETWORK_END		}

#define PACKET_START	namespace packet {
#define PACKET_END		}

NETWORK_START
constexpr int PORT_NUMBER = 4000;
constexpr int BUFFER_SIZE = 200;

constexpr const char* SERVER_IP = "127.0.0.1";
NETWORK_END

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

// Param
//	int playerId: Ŭ���̾�Ʈ�� �÷��̾� ���̵�
struct SCLogin : Header
{
	int playerId{ -1 };

	SCLogin(int playerId) :
		playerId{playerId}
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
	int playerId{ -1 };
	float x{ 0.f };
	float y{ 0.f };

	SCMovePlayer(const int playerId, const float x, const float y) :
		playerId{playerId},
		x{x},
		y{y}
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
	int playerId{ -1 };
	float x{ 0.f };
	float y{ 0.f };

	CSMovePlayer(const int playerId, const float x, const float y) :
		playerId{ playerId },
		x{ x },
		y{ y }
	{
		size = sizeof(CSMovePlayer);
		type = Type::CS_MOVE_PLAYER;
	}
};

#pragma pack(pop)
PACKET_END
