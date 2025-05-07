#pragma once
#include "BaseCollider.h"

enum class COLLISION_OBJECT_TYPE
{
	PLAYER,
	CRAB,
	DUMMY,
	BULLET,
	GENERATE,
};

class CollisionManager
{
	DECLARE_SINGLE(CollisionManager);

public:
	void RegisterCollider(const shared_ptr<BaseCollider>& collider, COLLISION_OBJECT_TYPE objectType);
	void UnregisterCollider(const shared_ptr<BaseCollider>& collider);

	void Update();

	void CheckPlayerToDummy();
	void CheckCrabToDummy();
	void CheckPlayerToCrab();
	void CheckMonsterToBullet();
	void CheckGenerateToBullet();

private:
	vector<pair<shared_ptr<BaseCollider>, COLLISION_OBJECT_TYPE>> _colliders;
};