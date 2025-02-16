#pragma once

enum class PacketType : unsigned char
{
	CS_LOGIN,
	SC_LOGIN
};

#pragma pack(push, 1)
struct BasicPacket
{
	unsigned short	size;
	PacketType		type;
};



struct CSLoginPacket : BasicPacket
{
	CSLoginPacket()
	{
		type = PacketType::CS_LOGIN;
	}
};


struct SCLoginPacket : BasicPacket
{
	SCLoginPacket()
	{
		type = PacketType::SC_LOGIN;
	}
};
#pragma pack(pop)

