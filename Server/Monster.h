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


	// getter /setter
	ObjectType GetObjectType() const override { return ObjectType::Monster; }

	void SetMonsterType(const MonsterType type) { _monsterType = type; }

	// 자동적으로 Collider 생성.
	// Todo: 
	// 일단 이렇게 하는데 나중에 SETMONSTERTYPE에서 바뀌도록 해야한다.
	Monster()
	{
		SetCollider(std::make_shared<AABBCollider>(GetPosRef(), MONSTER_CRAB_SIZE_M, MONSTER_CRAB_SIZE_M));
	}

private:

	MonsterType			_monsterType{ MonsterType::None };
	
};

