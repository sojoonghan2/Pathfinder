#pragma once
#define UTIL_START	namespace util {
#define UTIL_END	}

UTIL_START

// ���� �Լ� ���� ��� �� ����
void DisplayQuitError();

// ���� �Լ� ���� ��
void DisplayError();

class Timer {
private:
	using Clock = std::chrono::steady_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using MilliSeconds = std::chrono::duration<float, std::milli>; // �и��� ������ ����

	TimePoint lastFrame;
	float deltaTimeMS; // �и��� ������ ��ŸŸ��

public:
	Timer() : lastFrame(Clock::now()), deltaTimeMS(0.0f) {}

	float getDeltaTimeMS() const { return deltaTimeMS; }
	float getDeltaTimeSeconds() const { return deltaTimeMS / 1000.0f; } // �� ���� ��ȯ

	void updateDeltaTime();

	float PeekDeltaTime() const;
};

UTIL_END