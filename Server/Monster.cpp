#include "pch.h"
#include "Monster.h"
#include "Room.h"

// ·£´ý ¿£Áø
std::random_device rd;
std::mt19937 gen{ rd() };
std::uniform_real_distribution<float> speed_dist{ 0.667f, 1.5f };

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
	case MonsterType::CRAB:
	{
		SetCollider(std::make_shared<AABBCollider>(GetPosRef(), MONSTER_CRAB_SIZE_M, MONSTER_CRAB_SIZE_M));
		auto speed{ speed_dist(gen) * MONSTER_CRAB_SPEED_MPS };
		SetSpeed(MONSTER_CRAB_SPEED_MPS);
		_hp = MONSTER_CRAB_HP;
	}
	break;
	default:
		break;
	}
}
