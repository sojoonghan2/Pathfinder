#pragma once

// 상수 정의

// MAP 
constexpr float MAP_SIZE_M{ 50.f };


// PLAYER
constexpr float PLAYER_SIZE_M{ 0.5f };
constexpr float PLAYER_SPEED_MPS{ 5.f };

constexpr float SKILL_DASH_SPEED_MPS{ 50.f };
constexpr float SKILL_DASH_COOLDOWN_S{ 1.f };
constexpr float SKILL_GRENADE_COOLDOWN_S{ 10.f };
constexpr float SKILL_RAZER_COOLDOWN_S{ 10.f };







// NETWORK
constexpr int PORT_NUMBER{ 4000 };
constexpr int BUFFER_SIZE{ 200 };

constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float MOVE_PACKET_TIME_MS{ 75.f }; // 초당 13.3회
constexpr float MAX_NETWORK_DELAY_MS{ 200.f }; // 최대 네트워크 딜레이

constexpr int MAX_PLAYER{ 30000 };
constexpr int MAX_ROOM{ 10100 };

#define PACKET_START	namespace packet {
#define PACKET_END		}

PACKET_START
enum class Type : unsigned char
{
	NONE,
	SC_LOGIN,
	CS_LOGIN,
	SC_MATCHMAKING,
	CS_MATCHMAKING,
	SC_MOVE_PLAYER,
	CS_MOVE_PLAYER,
	SC_CHECK_DELAY,
	CS_CHECK_DELAY
};

#pragma pack(push, 1)
struct Header
{
	unsigned char	size{ sizeof(Header) };
	Type			type{ Type::NONE };

	Header() = default;
	Header(unsigned char size, Type type) :
		size{ size },
		type{ type }
	{}
};



// NO Param
struct SCLogin : Header
{
	SCLogin(int client_id) :
		Header{ sizeof(SCLogin), Type::SC_LOGIN }
	{}
};

// No Param
struct CSLogin : Header
{
	CSLogin() :
		Header{ sizeof(CSLogin), Type::CS_LOGIN }
	{}
};




// Param
//	int playerId: 클라이언트의 플레이어 아이디
struct SCMatchmaking : Header
{
	int clientId{ -1 };

	SCMatchmaking(int client_id) :
		Header{ sizeof(SCMatchmaking), Type::SC_MATCHMAKING },
		clientId{ client_id }
	{}
};

// No Param
struct CSMatchmaking : Header
{
	CSMatchmaking() :
		Header{ sizeof(CSMatchmaking), Type::CS_MATCHMAKING }
	{}
};

// Param:
//	int clientId
//	float x
//	float y
struct SCMovePlayer : Header
{
	int clientId{ -1 };
	float x{ 0.f };
	float y{ 0.f };

	SCMovePlayer(const int client_id, const float x, const float y) :
		Header{ sizeof(SCMovePlayer), Type::SC_MOVE_PLAYER },
		clientId{client_id},
		x{x},
		y{y}
	{}
};

// Param:
//	float x
//	float y
struct CSMovePlayer : Header
{
	float x{ 0.f };
	float y{ 0.f };

	CSMovePlayer(const float x, const float y) :
		Header{ sizeof(CSMovePlayer), Type::CS_MOVE_PLAYER },
		x{ x },
		y{ y }
	{}
};

// No Param
struct SCCheckDelayPacket : Header
{
	SCCheckDelayPacket() :
		Header{ sizeof(SCCheckDelayPacket), Type::SC_CHECK_DELAY }
	{}
};

// No Param
struct CSCheckDelayPacket : Header
{
	CSCheckDelayPacket() :
		Header{ sizeof(CSCheckDelayPacket), Type::CS_CHECK_DELAY }		
	{}
};


#pragma pack(pop)
PACKET_END
