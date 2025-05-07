#pragma once
#include "BaseCollider.h"

enum class COLLISION_OBJECT_TYPE
{
	PLAYER,
	MONSTER,
	DUMMY,
};

class CollisionManager
{
	DECLARE_SINGLE(CollisionManager);

public:
	void RegisterCollider(const shared_ptr<BaseCollider>& collider, COLLISION_OBJECT_TYPE objectType);
	void UnregisterCollider(const shared_ptr<BaseCollider>& collider);

	void Update();

	void CheckPlayerToDummy();
	void CheckMonsterToDummy();

private:
	vector<pair<shared_ptr<BaseCollider>, COLLISION_OBJECT_TYPE>> _colliders;
};