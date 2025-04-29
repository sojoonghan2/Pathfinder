#pragma once
#include "Object.h"
#include "AABBCollider.h"


enum class MonsterType : unsigned char
{
	None = 0,
	Crab,
	Tank,
	Robot,
	Core
};

// 나중에 상속 구조로 변경
class Monster : public Object
{
public:
	bool GetRunning() const;

	// CAS를 사용해서 동작.
	bool TrySetRunning(const bool running);

	void Update(const float delta_time);


	// getter /setter
	ObjectType GetObjectType() const override { return ObjectType::Monster; }

	void SetMonsterType(const MonsterType type) { _monsterType = type; }
	void SetRoomId(const int room_id) { _roomId = room_id; }
	
	int GetRoomId() const { return _roomId; }

	// 자동적으로 Collider 생성.
	// TODO: MonsterType에 따라 달라지도록. 지금은 CRAB 고정.
	Monster()
	{
		SetCollider(std::make_shared<AABBCollider>(GetPosRef(), MONSTER_CRAB_SIZE_M, MONSTER_CRAB_SIZE_M));
	}

private:
	int		_roomId{ -1 };

	std::atomic_bool	_isRunning{ false };
	MonsterType			_monsterType{ MonsterType::None };
};

