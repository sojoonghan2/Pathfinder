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
	// Ÿ�̸� ���ļ��� ����, �ػ󵵸� ����
	uint64	_frequency = 0;
	// ���� �������� ī��Ʈ �� ����, ��Ÿ Ÿ�� ���
	uint64	_prevCount = 0;
	// ������ �� �ð� ����(��Ÿ Ÿ��)
	float	_deltaTime = 0.f;

private:
	// ���� ������ ��
	uint32	_frameCount = 0;
	// ������ �������� �ð�
	float	_frameTime = 0.f;
	// �ʴ� ������ ��(FPS)
	uint32	_fps = 0;
};

