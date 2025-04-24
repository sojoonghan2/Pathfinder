#pragma once
#include "Object.h"


enum class MonsterType : unsigned char
{
	None = 0,
	Crab,
	Tank,
	Robot,
	Core
};

// ���߿� ��� ������ ����
class Monster : public Object
{
public:
	bool GetRunning() const;

	// CAS�� ����ؼ� ����.
	bool TrySetRunning(const bool running);

	void Update(const float delta_time);


	// getter /setter
	void SetMonsterType(const MonsterType type) { _monsterType = type; }
	void SetRoomId(const int room_id) { _roomId = room_id; }
	void SetSpeed(const float speed) { _speed = speed; }
	
	int GetRoomId() const { return _roomId; }

private:
	int		_roomId{ -1 };
	float	_speed{ 0.f };


	std::atomic_bool _isRunning{ false };
	MonsterType _monsterType{ MonsterType::None };
};

