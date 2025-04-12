#include "pch.h"
#include "IOCP.h"
#include "Game.h"
#include "Timer.h"

int main()
{
	

	GET_SINGLE(Game)->Init();
	auto ret = GET_SINGLE(IOCP)->Init();
	if (false == ret) {
		return -1;
	}

	GET_SINGLE(IOCP)->Start();

	Timer timer;
	while (true) {
		std::this_thread::yield();
	}
	
}