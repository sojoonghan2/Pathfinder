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

// ������Ʈ�� �ݶ��̴��� �̰��� ����ϰ� ���� �� ��ȣ�ۿ�
class CollisionManager
{
	DECLARE_SINGLE(CollisionManager);

public:
	void RegisterCollider(const shared_ptr<BaseCollider>& collider, COLLISION_OBJECT_TYPE objectType);
	void UnregisterCollider(const shared_ptr<BaseCollider>& collider);
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

private:
	vector<pair<shared_ptr<BaseCollider>, COLLISION_OBJECT_TYPE>> _colliders;
};