#include "pch.h"
#include "IOCP.h"

int main()
{
	
	IOCP iocp;
	
	auto ret = iocp.InitServer();
	if (false == ret) {
		return -1;
	}

	iocp.StartServer();
	
}