#include "pch.h"
#include "NetworkTimer.h"

NETWORK_START


void NetworkTimer::updateDeltaTime()
{
	TimePoint currentFrame = Clock::now();
	deltaTimeMS = std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
	lastFrame = currentFrame;
}

float NetworkTimer::PeekDeltaTime() const
{
	TimePoint currentFrame = Clock::now();
	return std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
}

NETWORK_END