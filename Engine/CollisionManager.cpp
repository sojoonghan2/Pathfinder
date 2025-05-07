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
	CheckMonsterToBullet();
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
	for (auto& [monster, typeA] : _colliders)
	{
		if (typeA != COLLISION_OBJECT_TYPE::CRAB || !monster->IsEnabled())
			continue;

		for (auto& [dummy, typeB] : _colliders)
		{
			if (typeB != COLLISION_OBJECT_TYPE::DUMMY || !dummy->IsEnabled())
				continue;

			const auto& objA = monster->GetGameObject();
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
}

void CollisionManager::CheckMonsterToBullet()
{
}

void CollisionManager::CheckGenerateToBullet()
{
}
