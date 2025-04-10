#pragma once

enum class MonsterType : unsigned char
{
	None = 0,
	Crab,
	Tank,
	Robot,
	Core
};

// 나중에 상속 구조로 변경
class Monster
{
public:
	void Move(const Vec2f& pos);
	void Move(const float x, const float y);

	bool GetRunning() const;
	bool TrySetRunning(const bool running);

	void Update();

	void SetMonsterType(MonsterType type) { _monsterType = type; }

private:
	Vec2f	_pos{};
	float&	_x{ _pos.x };
	float&	_y{ _pos.y };
	int		_roomId{ -1 };
	float	_speed{ 0.f };


	std::atomic_bool _isRunning{ false };

	MonsterType _monsterType{ MonsterType::None };
};

