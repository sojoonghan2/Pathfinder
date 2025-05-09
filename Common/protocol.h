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

// MONSTER
constexpr float MONSTER_CRAB_SIZE_M{ 0.5f };
constexpr float MONSTER_CRAB_SPEED_MPS{ 2.f };

// other object
constexpr float BULLET_SPEED_MPS{ 15.f };


// NETWORK
constexpr int PORT_NUMBER{ 4000 };
constexpr int BUFFER_SIZE{ 200 };

constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float MOVE_PACKET_TIME_MS{ 50.f }; // 초당 20회
constexpr float MAX_NETWORK_DELAY_MS{ 100.f }; // 최대 네트워크 딜레이

constexpr int MAX_PLAYER{ 100 };
constexpr int MAX_ROOM{ MAX_PLAYER / 3 + 1 };
constexpr int MAX_MONSTER{ MAX_ROOM * 10 };
constexpr int MAX_BULLET{ MAX_ROOM * 30 };

enum class RoomType : unsigned char
{
	None = 0,
	Ruin,
	Factory,
	Ristrict,
	Falling,
	Lucky
};

enum class ObjectType : unsigned char
{
	None = 0,
	Player,
	MainPlayer, // for client. not use in server
	Monster,
	Bullet,
};

#define PACKET_START	namespace packet {
#define PACKET_END		}

PACKET_START
enum class Type : unsigned char
{
	NONE = 0,

	// prepare
	SC_LOGIN,
	CS_LOGIN,
	SC_MATCHMAKING,
	CS_MATCHMAKING,
	CS_LOAD_COMPLETE,
	SC_GAME_START,	

	// move
	SC_MOVE_OBJECT,
	CS_MOVE_PLAYER,

	SC_DELETE_OBJECT,

	// player skills
	CS_FIRE_BULLET,

	// other
	SC_CHECK_DELAY,
	CS_CHECK_DELAY,
};

#pragma pack(push, 1)
struct Header
{
	unsigned char	size{ sizeof(Header) };
	Type			type{ Type::NONE };

	Header() = default;
	Header(const unsigned char size, const Type type) :
		size{ size },
		type{ type }
	{}
};


// NO Param
struct SCLogin : Header
{
	SCLogin() :
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


// TODO: 여기서 본인의 클라이언트 아이디가 아닌 플레이어 아이디를 줘야 함.

// Param
//	int playerId: 클라이언트의 플레이어 아이디
//  roomType
struct SCMatchmaking : Header
{
	int playerId{ -1 };
	RoomType roomType{ RoomType::None };

	SCMatchmaking(const int player_id, const RoomType room_type) :
		Header{ sizeof(SCMatchmaking), Type::SC_MATCHMAKING },
		playerId{ player_id },
		roomType{ room_type }
	{}
};

// No Param
struct CSMatchmaking : Header
{
	CSMatchmaking() :
		Header{ sizeof(CSMatchmaking), Type::CS_MATCHMAKING }
	{}
};


// No Param
struct CSLoadComplete : Header
{
	CSLoadComplete() :
		Header{ sizeof(CSLoadComplete), Type::CS_LOAD_COMPLETE }
	{}
};

// No Param
struct SCGameStart : Header
{
	SCGameStart() :
		Header{ sizeof(SCGameStart), Type::SC_GAME_START }
	{}
};

// TODO: SCMoveObject로 통일.
// CSMovePlayer는 그대로 두자.

// Param:
//	int monsterId
//	float x
//	float y
//	float dirX
//	float dirY
struct SCMoveObject : Header
{
	int objectId{ -1 };
	ObjectType objectType{ ObjectType::None };
	float x{ 0.f };
	float y{ 0.f };
	float dirX{ 0.f };
	float dirY{ 0.f };

	SCMoveObject(const int object_id, const ObjectType object_type,
		const float x, const float y, const float dirX, const float dirY) :
		Header{ sizeof(SCMoveObject), Type::SC_MOVE_OBJECT },
		objectId{ object_id },
		objectType{ object_type },
		x{ x }, y{ y },
		dirX{ dirX }, dirY{ dirY }
	{}
};

// Param:
//	float x
//	float y
//	float dirX
//	float dirY
struct CSMovePlayer : Header
{
	float x{ 0.f };
	float y{ 0.f };
	float dirX{ 0.f };
	float dirY{ 0.f };


	CSMovePlayer(const float x, const float y, const float dirX, const float dirY) :
		Header{ sizeof(CSMovePlayer), Type::CS_MOVE_PLAYER },
		x{ x }, y{ y },
		dirX{ dirX }, dirY{ dirY }
	{}
};

struct CSFireBullet : Header
{
	CSFireBullet() :
		Header{ sizeof(CSFireBullet), Type::CS_FIRE_BULLET }
	{}
};

struct SCDeleteObject : Header
{
	int objectId{ -1 };

	SCDeleteObject(const int object_id) :
		Header{ sizeof(SCDeleteObject), Type::SC_DELETE_OBJECT },
		objectId{ object_id }
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
