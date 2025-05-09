#include "pch.h"
#include "Monster.h"
#include "Room.h"

// ·£´ý ¿£Áø

void Monster::Update(const float delta)
{
	Move(delta);
}

void Monster::InitMonster(const MonsterType monster_type, const Vec2f pos)
{
	_monsterType = monster_type;
	SetPos(pos);

	switch (monster_type)
	{
	case MonsterType::Crab:
	{
		SetCollider(std::make_shared<AABBCollider>(GetPosRef(), MONSTER_CRAB_SIZE_M, MONSTER_CRAB_SIZE_M));
		SetSpeed(MONSTER_CRAB_SPEED_MPS);
	}
	break;
	default:
		break;
	}
}
