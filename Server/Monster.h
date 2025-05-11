#pragma once
#include "Object.h"
#include "AABBCollider.h"

// ���߿� ��� ������ ����
class Monster : public Object
{
public:
	
	void Update(const float delta) override;

	void InitMonster(const MonsterType monster_type, const Vec2f pos);

	void DecreaseHp(const float hp) { _hp -= hp; }
	bool IsDead() const { return _hp <= EPSILON; }

	// getter /setter
	ObjectType GetObjectType() const override { return ObjectType::MONSTER; }
	float GetHp() const { return _hp; }
private:

	MonsterType		_monsterType{ MonsterType::NONE };
	float			_hp{ 0.f };
};

