#pragma once

// ��� ����

// ���� m
constexpr float MAP_SIZE_M{ 50.f };
constexpr float PLAYER_SIZE_M{ 0.5f };

// �÷��̾� �̵��ӵ�, ���� m/s
constexpr float PLAYER_SPEED_MPS{ 5.f };


constexpr int PORT_NUMBER{ 4000 };
constexpr int BUFFER_SIZE{ 200 };

constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float MOVE_PACKET_TIME_MS{ 75.f }; // �ʴ� 13.3ȸ
constexpr float MAX_NETWORK_DELAY_MS{ 200.f }; // �ִ� ��Ʈ��ũ ������

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
	SC_MOVE_PLAYER,
	CS_MOVE_PLAYER,
	SC_CHECK_DELAY,
	CS_CHECK_DELAY
};

#pragma pack(push, 1)
struct Header
{
	unsigned short	size{ sizeof(Header) };
	Type			type{ Type::NONE };
};



// Param
//	int playerId: Ŭ���̾�Ʈ�� �÷��̾� ���̵�
struct SCLogin : Header
{
	int clientId{ -1 };

	SCLogin(int client_id) :
		clientId{ client_id }
	{
		size = sizeof(SCLogin);
		type = Type::SC_LOGIN;
	}
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
//	float x
//	float y
struct CSMovePlayer : Header
{
	float x{ 0.f };
	float y{ 0.f };

	CSMovePlayer(const float x, const float y) :
		x{ x },
		y{ y }
	{
		size = sizeof(CSMovePlayer);
		type = Type::CS_MOVE_PLAYER;
	}
};

// No Param
struct SCCheckDelayPacket : Header
{
	SCCheckDelayPacket()
	{
		size = sizeof(SCCheckDelayPacket);
		type = Type::SC_CHECK_DELAY;
	}
};

// No Param
struct CSCheckDelayPacket : Header
{
	CSCheckDelayPacket()
	{
		size = sizeof(CSCheckDelayPacket);
		type = Type::CS_CHECK_DELAY;
	}
};


#pragma pack(pop)
PACKET_END
