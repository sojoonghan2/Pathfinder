#pragma once
#pragma once


class NetworkTimer
{
private:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using MilliSeconds = std::chrono::duration<float, std::milli>;

	TimePoint lastFrame;
	float deltaTimeMS; // �и��� ������ ��ŸŸ��

public:
	NetworkTimer() : lastFrame(Clock::now()), deltaTimeMS(0.0f) {}

	float getDeltaTimeMS() const { return deltaTimeMS; }
	float getDeltaTimeSeconds() const { return deltaTimeMS / 1000.0f; } // �� ���� ��ȯ

	void updateDeltaTime();

	float PeekDeltaTime() const;
};

