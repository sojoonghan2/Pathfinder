#include "pch.h"
#include "Monster.h"

void Monster::Move(const Vec2f& pos)
{
	_pos = pos;
}

void Monster::Move(const float x, const float y)
{
	_x = x;
	_y = y;
}

void Monster::Update()
{
	// ���� ����� �÷��̾��� ��ġ�� ã�Ƽ�
	// �� �������� �̵��ϴ� ������ �����ؾ� ��.
}


bool Monster::TrySetRunning(const bool running)
{
	bool expected = not running;
	return _isRunning.compare_exchange_strong(expected, running);
}

bool Monster::GetRunning() const
{
	return _isRunning.load();
}
