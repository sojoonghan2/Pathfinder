#include "pch.h"
#include "Timer.h"

void Timer::Init()
{
	_firstTime = Clock::now();
	_prevTime = _firstTime;
	_elapsedTime = 0.0f;
	_totalTime = 0.0f;
	_deltaTime = 0.0f;
	_frameCount = 0;
	_fps = 0;
	_stopped = false;
}

void Timer::Update(float lockFPS)
{
	if (_stopped)
	{
		_deltaTime = 0.f;
		return;
	}

	TimePoint current = Clock::now();
	std::chrono::duration<float> delta = current - _prevTime;

	// 고정 FPS 처리
	if (lockFPS > 0.f)
	{
		const float minFrameTime = 1.f / lockFPS;
		while (delta.count() < minFrameTime)
		{
			current = Clock::now();
			delta = current - _prevTime;
		}
	}

	_prevTime = current;
	_deltaTime = delta.count();
	_totalTime = std::chrono::duration<float>(current - _firstTime).count();

	_frameCount++;
	_elapsedTime += _deltaTime;

	if (_elapsedTime >= 1.0f)
	{
		_fps = _frameCount;
		_frameCount = 0;
		_elapsedTime = 0.0f;
	}
}

void Timer::Reset()
{
	Init();
}

void Timer::Stop()
{
	if (!_stopped)
	{
		_stopTime = Clock::now();
		_stopped = true;
	}
}

void Timer::Start()
{
	if (_stopped)
	{
		TimePoint resumeTime = Clock::now();
		_firstTime += (resumeTime - _stopTime);
		_prevTime = resumeTime;
		_stopped = false;
	}
}
