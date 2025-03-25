#pragma once

// 상수 정의

// 단위 m
constexpr float MAP_SIZE_M{ 50.f };
constexpr float PLAYER_SIZE_M{ 0.5f };

// 플레이어 이동속도, 단위 m/s
constexpr float PLAYER_SPEED_MPS{ 5.f };


constexpr int PORT_NUMBER{ 4000 };
constexpr int BUFFER_SIZE{ 200 };

constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float MOVE_PACKET_TIME_MS{ 75.f }; // 초당 13.3회
constexpr float MAX_NETWORK_DELAY_MS{ 200.f }; // 최대 네트워크 딜레이

constexpr int MAX_PLAYER{ 3000 };
constexpr int MAX_ROOM{ 1000 };

#define PACKET_START	namespace packet {
#define PACKET_END		}

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
//	int playerId: 클라이언트의 플레이어 아이디
struct SCLogin : Header
{
	int clientId{ -1 };

	SCLogin(int client_id) :
		clientId{client_id}
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
	int clientId{ -1 };
	float x{ 0.f };
	float y{ 0.f };

	SCMovePlayer(const int client_id, const float x, const float y) :
		clientId{client_id},
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
	int clientId{ -1 };
	float x{ 0.f };
	float y{ 0.f };

	CSMovePlayer(const int client_id, const float x, const float y) :
		clientId{ client_id },
		x{ x },
		y{ y }
	{
		size = sizeof(CSMovePlayer);
		type = Type::CS_MOVE_PLAYER;
	}
};

#pragma pack(pop)
PACKET_END
