#pragma once
#pragma once


class NetworkTimer
{
private:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using MilliSeconds = std::chrono::duration<float, std::milli>;

	TimePoint lastFrame;
	float deltaTimeMS; // 밀리초 단위의 델타타임

public:
	NetworkTimer() : lastFrame(Clock::now()), deltaTimeMS(0.0f) {}

	float getDeltaTimeMS() const { return deltaTimeMS; }
	float getDeltaTimeSeconds() const { return deltaTimeMS / 1000.0f; } // 초 단위 변환

	void updateDeltaTime();

	float PeekDeltaTime() const;
};

