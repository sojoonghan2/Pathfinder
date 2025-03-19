#pragma once
#define UTIL_START	namespace util {
#define UTIL_END	}

UTIL_START

// 소켓 함수 오류 출력 후 종료
void DisplayQuitError();

// 소켓 함수 오류 출
void DisplayError();

class Timer {
private:
	using Clock = std::chrono::steady_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using MilliSeconds = std::chrono::duration<float, std::milli>; // 밀리초 단위로 변경

	TimePoint lastFrame;
	float deltaTimeMS; // 밀리초 단위의 델타타임

public:
	Timer() : lastFrame(Clock::now()), deltaTimeMS(0.0f) {}

	float getDeltaTimeMS() const { return deltaTimeMS; }
	float getDeltaTimeSeconds() const { return deltaTimeMS / 1000.0f; } // 초 단위 변환

	void updateDeltaTime();

	float PeekDeltaTime() const;
};

UTIL_END