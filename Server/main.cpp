#include "pch.h"
#include "IOCP.h"
#include "Game.h"

int main()
{
	
	IOCP iocp;
	
	auto ret = iocp.Init();
	if (false == ret) {
		return -1;
	}

	GET_SINGLE(Game)->Init();

	iocp.Start();

	// game loop
	while (true) {

	}
	
}