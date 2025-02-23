#include "pch.h"
#include "IOCP.h"

int main()
{
	
	IOCP iocp;
	
	auto ret = iocp.Init();
	if (false == ret) {
		return -1;
	}

	iocp.Start();
	
}