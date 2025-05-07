#include "pch.h"
#include "CollisionManager.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "PlayerScript.h"
#include "CrabScript.h"

void CollisionManager::RegisterCollider(const shared_ptr<BaseCollider>& collider, COLLISION_OBJECT_TYPE objectType)
{
	for (auto& [c, t] : _colliders)
	{
		if (c == collider)
			return;
	}

	_colliders.emplace_back(collider, objectType);
}

void CollisionManager::UnregisterCollider(const shared_ptr<BaseCollider>& collider)
{
	_colliders.erase(remove_if(_colliders.begin(), _colliders.end(),
		[&](const auto& pair)
		{
			return pair.first == collider;
		}), _colliders.end());
}

void CollisionManager::Update()
{
	CheckPlayerToDummy();
	CheckCrabToDummy();
	CheckPlayerToCrab();
	CheckCrabToBullet();
	CheckCrabToGrenade();
	CheckCrabToRazer();
	CheckGenerateToBullet();
}

void CollisionManager::CheckPlayerToDummy()
{
	for (auto& [player, typeA] : _colliders)
	{
		if (typeA != COLLISION_OBJECT_TYPE::PLAYER || !player->IsEnabled())
			continue;

		for (auto& [dummy, typeB] : _colliders)
		{
			if (typeB != COLLISION_OBJECT_TYPE::DUMMY || !dummy->IsEnabled())
				continue;

			const auto& objA = player->GetGameObject();
			const auto& objB = dummy->GetGameObject();

			if (!objA || !objB)
				continue;

			if (GET_SINGLE(SceneManager)->Collition(objA, objB))
			{
				auto playerScript = objA->GetScript<PlayerScript>();
				playerScript->CheckDummyHits(objB);
			}
		}
	}
}

void CollisionManager::CheckCrabToDummy()
{
	for (auto& [crab, typeA] : _colliders)
	{
		if (typeA != COLLISION_OBJECT_TYPE::CRAB || !crab->IsEnabled())
			continue;

		for (auto& [dummy, typeB] : _colliders)
		{
			if (typeB != COLLISION_OBJECT_TYPE::DUMMY || !dummy->IsEnabled())
				continue;

			const auto& objA = crab->GetGameObject();
			const auto& objB = dummy->GetGameObject();

			if (!objA || !objB)
				continue;

			if (GET_SINGLE(SceneManager)->Collition(objA, objB))
			{
				auto crabScript = objA->GetScript<CrabScript>();
				crabScript->CheckDummyHits(objB);
			}
		}
	}
}

void CollisionManager::CheckPlayerToCrab()
{
	for (auto& [player, typeA] : _colliders)
	{
		if (typeA != COLLISION_OBJECT_TYPE::PLAYER || !player->IsEnabled())
			continue;

		for (auto& [dummy, typeB] : _colliders)
		{
			if (typeB != COLLISION_OBJECT_TYPE::CRAB || !dummy->IsEnabled())
				continue;

			const auto& objA = player->GetGameObject();
			const auto& objB = dummy->GetGameObject();

			if (!objA || !objB)
				continue;

			if (GET_SINGLE(SceneManager)->Collition(objA, objB))
			{
				auto playerScript = objA->GetScript<PlayerScript>();
				playerScript->CheckCrabHits();
			}
		}
	}
}

void CollisionManager::CheckCrabToBullet()
{
	for (auto& [crab, typeA] : _colliders)
	{
		if (typeA != COLLISION_OBJECT_TYPE::CRAB || !crab->IsEnabled())
			continue;

		for (auto& [bullet, typeB] : _colliders)
		{
			if (typeB != COLLISION_OBJECT_TYPE::BULLET || !bullet->IsEnabled())
				continue;

			const auto& objA = crab->GetGameObject();
			const auto& objB = bullet->GetGameObject();

			if (!objA || !objB)
				continue;

			if (GET_SINGLE(SceneManager)->Collition(objA, objB))
			{
				auto crabScript = objA->GetScript<CrabScript>();
				crabScript->CheckBulletHits(objB);
			}
		}
	}
}

void CollisionManager::CheckCrabToGrenade()
{
	for (auto& [crab, typeA] : _colliders)
	{
		if (typeA != COLLISION_OBJECT_TYPE::CRAB || !crab->IsEnabled())
			continue;

		for (auto& [grenade, typeB] : _colliders)
		{
			if (typeB != COLLISION_OBJECT_TYPE::GRENADE || !grenade->IsEnabled())
				continue;

			const auto& objA = crab->GetGameObject();
			const auto& objB = grenade->GetGameObject();

			if (!objA || !objB)
				continue;

			if (GET_SINGLE(SceneManager)->Collition(objA, objB))
			{
				auto crabScript = objA->GetScript<CrabScript>();
				crabScript->CheckGrenadeHits();
			}
		}
	}
}

void CollisionManager::CheckCrabToRazer()
{
}

void CollisionManager::CheckGenerateToBullet()
{
}