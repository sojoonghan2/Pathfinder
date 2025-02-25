#pragma once

#define PACKET_START namespace packet {
#define PACKET_END }


PACKET_START
enum class Type : unsigned char
{
	NONE,
	CS_LOGIN,
	SC_LOGIN
};

#pragma pack(push, 1)
struct Header
{
	unsigned short	size{ sizeof(Header) };
	Type			type{ Type::NONE };
};


struct CSLogin : Header
{
	CSLogin()
	{
		size = sizeof(CSLogin);
		type = Type::CS_LOGIN;
	}
};


struct SCLogin : Header
{
	SCLogin()
	{
		size = sizeof(SCLogin);
		type = Type::SC_LOGIN;
	}
};
#pragma pack(pop)
PACKET_END
