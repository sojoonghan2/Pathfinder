#pragma once


enum class MonsterType : unsigned char
{
	None = 0,
	Crab,
	Tank,
	Robot,
	Core
};

// ���߿� ��� ������ ����
class Monster
{
public:
	void Move(const Vec2f& pos);
	void Move(const float x, const float y);

	bool GetRunning() const;

	// CAS�� ����ؼ� ����.
	bool TrySetRunning(const bool running);

	void Update(const float delta_time);







	// getter /setter
	void SetMonsterType(const MonsterType type) { _monsterType = type; }
	void SetRoomId(const int room_id) { _roomId = room_id; }
	void SetSpeed(const float speed) { _speed = speed; }
	
	// ���������� normalize��.
	void SetDir(const Vec2f& dir);

	Vec2f GetPos() const { return _pos; }
	int GetRoomId() const { return _roomId; }

private:
	Vec2f	_pos{};
	float&	_x{ _pos.x };
	float&	_y{ _pos.y };

	// normalize�ؾ� ��.
	Vec2f	_dir{ 0.f, 0.f };

	int		_roomId{ -1 };

	float	_speed{ 0.f };


	std::atomic_bool _isRunning{ false };

	MonsterType _monsterType{ MonsterType::None };
};

