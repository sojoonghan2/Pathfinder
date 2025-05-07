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

// ���߿� ��� ������ ����
class Monster : public Object
{
public:


	// getter /setter
	ObjectType GetObjectType() const override { return ObjectType::Monster; }

	void Update(const float delta) override;

	void InitMonster(const MonsterType monster_type, const Vec2f pos);

private:

	MonsterType			_monsterType{ MonsterType::None };
	
};

