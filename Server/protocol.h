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
};


struct SCLoginPacket : BasicPacket
{
};
#pragma pack(pop)

