#pragma once
#include <chrono>

// 타이머
// FPS를 60으로 Lock
class Timer
{
	DECLARE_SINGLE(Timer);

public:
	void Init();
	void Update(float lockFPS = 0.f);
	void Reset();
	void Start();
	void Stop();

	float GetDeltaTime() const { return _deltaTime; }
	float GetElapsedTime() const { return _elapsedTime; }
	float GetTotalTime() const { return _totalTime; }
	int32_t GetFps() const { return _fps; }
	bool IsStopped() const { return _stopped; }

private:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	TimePoint _firstTime{};
	TimePoint _prevTime{};
	TimePoint _stopTime{};

	float _deltaTime = 0.0f;
	float _elapsedTime = 0.0f;
	float _totalTime = 0.0f;

	int32_t _frameCount = 0;
	int32_t _fps = 0;

	bool _stopped = false;
};