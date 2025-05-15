#pragma once
#include "BaseCollider.h"

enum class COLLISION_OBJECT_TYPE
{
	PLAYER,
	CRAB,
	DUMMY,
	BULLET,
	GRENADE,
	GENERATE,
	FACTORYMID,
};

struct RayInfo
{
	Vec4 origin;
	Vec4 direction;
};

// 오브젝트의 콜라이더를 이곳에 등록하고 관리 및 상호작용
class CollisionManager
{
	DECLARE_SINGLE(CollisionManager);

public:
	void RegisterCollider(const shared_ptr<BaseCollider>& collider, COLLISION_OBJECT_TYPE objectType);
	void UnregisterCollider(const shared_ptr<BaseCollider>& collider);
	void RegisterRay(const Vec4& origin, const Vec4& direction);

	void ClearCollider();

	void Update();

	void CheckPlayerToDummy();
	void CheckCrabToDummy();
	void CheckPlayerToCrab();
	void CheckCrabToBullet();
	void CheckCrabToGrenade();
	void CheckCrabToRazer();
	void CheckGenerateToBullet();
	void CheckFactoryMidToBullet();

	void CheckRayToCrab();

private:
	vector<pair<shared_ptr<BaseCollider>, COLLISION_OBJECT_TYPE>> _colliders;
	vector<RayInfo> _rays;
};