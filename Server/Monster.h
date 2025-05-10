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
	
	void Update(const float delta) override;

	void InitMonster(const MonsterType monster_type, const Vec2f pos);

	void DecreaseHp(const int hp) { _hp -= hp; }
	bool IsDead() const { return _hp <= 0; }

	// getter /setter
	ObjectType GetObjectType() const override { return ObjectType::Monster; }
	int GetHp() const { return _hp; }
private:

	MonsterType		_monsterType{ MonsterType::None };
	int				_hp{ 0 };
};

