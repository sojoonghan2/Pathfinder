#pragma once
#include "Object.h"
#include "SweptCollider.h"

class Bullet : public Object
{
public:
	ObjectType GetObjectType() const override { return ObjectType::Bullet; }

	Bullet()
	{
		// SetCollider(std::make_shared<SweptCollider>(GetPosRef(), MONSTER_CRAB_SIZE_M, MONSTER_CRAB_SIZE_M));
	}
};

