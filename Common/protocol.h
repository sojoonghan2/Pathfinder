#pragma once

#define PACKET_START namespace packet {
#define PACKET_END }


PACKET_START
enum class Type : unsigned char
{
	CS_LOGIN,
	SC_LOGIN
};

#pragma pack(push, 1)
struct Header
{
	unsigned short	size;
	Type			type;
};


struct CSLogin : Header
{
	CSLogin()
	{
		type = Type::CS_LOGIN;
	}
};


struct SCLogin : Header
{
	SCLogin()
	{
		type = Type::SC_LOGIN;
	}
};
#pragma pack(pop)
PACKET_END
