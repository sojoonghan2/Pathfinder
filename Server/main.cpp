#include "pch.h"
#include "IOCP.h"
#include "Game.h"

int main()
{
	

	GET_SINGLE(Game)->Init();
	auto ret = GET_SINGLE(IOCP)->Init();
	if (false == ret) {
		return -1;
	}

	GET_SINGLE(IOCP)->Start();

	while (true) {
		std::this_thread::yield();
	}
	
}