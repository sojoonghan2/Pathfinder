#include "pch.h"
#include "NetworkTimer.h"

NETWORK_START


void Timer::updateDeltaTime()
{
	TimePoint currentFrame = Clock::now();
	deltaTimeMS = std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
	lastFrame = currentFrame;
}

float Timer::PeekDeltaTime() const
{
	TimePoint currentFrame = Clock::now();
	return std::chrono::duration_cast<MilliSeconds>(currentFrame - lastFrame).count();
}

NETWORK_END