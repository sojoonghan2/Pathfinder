#pragma once

class Timer
{
	DECLARE_SINGLE(Timer);

public:
	void Init();
	void Update();

	uint32 GetFps() { return _fps; }
	float GetDeltaTime() { return _deltaTime; }

private:
	// 타이머 주파수를 저장, 해상도를 결정
	uint64	_frequency = 0;
	// 이전 프레임의 카운트 값 저장, 델타 타임 계산
	uint64	_prevCount = 0;
	// 프레임 간 시간 차이(델타 타임)
	float	_deltaTime = 0.f;

private:
	// 현재 프레임 수
	uint32	_frameCount = 0;
	// 마지막 프레임의 시간
	float	_frameTime = 0.f;
	// 초당 프레임 수(FPS)
	uint32	_fps = 0;
};

