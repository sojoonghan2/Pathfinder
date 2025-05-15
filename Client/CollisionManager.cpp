#include "pch.h"
#include "CollisionManager.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include "PlayerScript.h"
#include "CrabScript.h"
#include "GeneratorScript.h"
#include "FactoryMidScript.h"

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

void CollisionManager::RegisterRay(const Vec4& origin, const Vec4& direction)
{
	RayInfo ray = { origin, direction };
	_rays.push_back(ray);
}

void CollisionManager::ClearCollider()
{
	_colliders.clear();
}

void CollisionManager::Update()
{
	const auto& sceneName = GET_SINGLE(SceneManager)->GetActiveScene()->GetName();

	CheckPlayerToDummy();
	if (sceneName == L"RuinsScene")
	{
		CheckCrabToDummy();
		CheckPlayerToCrab();
		//CheckCrabToBullet();
		CheckCrabToGrenade();
		CheckCrabToRazer();
		CheckRayToCrab();
	}
	else if (sceneName == L"FactoryScene")
	{
		CheckGenerateToBullet();
		CheckFactoryMidToBullet();
	}
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
				crabScript->CheckBulletHits();
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
				crabScript->CheckGrenadeHits(objB);
			}
		}
	}
}

void CollisionManager::CheckCrabToRazer()
{
}

void CollisionManager::CheckGenerateToBullet()
{
	for (auto& [generate, typeA] : _colliders)
	{
		if (typeA != COLLISION_OBJECT_TYPE::GENERATE || !generate->IsEnabled())
			continue;

		for (auto& [bullet, typeB] : _colliders)
		{
			if (typeB != COLLISION_OBJECT_TYPE::BULLET || !bullet->IsEnabled())
				continue;

			const auto& objA = generate->GetGameObject();
			const auto& objB = bullet->GetGameObject();

			if (!objA || !objB)
				continue;

			if (GET_SINGLE(SceneManager)->Collition(objA, objB))
			{
				auto generatorScript = objA->GetScript<GeneratorScript>();
				generatorScript->CheckBulletHits(objB);
			}
		}
	}
}

void CollisionManager::CheckFactoryMidToBullet()
{
	for (auto& [factorymid, typeA] : _colliders)
	{
		if (typeA != COLLISION_OBJECT_TYPE::FACTORYMID || !factorymid->IsEnabled())
			continue;

		for (auto& [bullet, typeB] : _colliders)
		{
			if (typeB != COLLISION_OBJECT_TYPE::BULLET || !bullet->IsEnabled())
				continue;

			const auto& objA = factorymid->GetGameObject();
			const auto& objB = bullet->GetGameObject();

			if (!objA || !objB)
				continue;

			if (GET_SINGLE(SceneManager)->Collition(objA, objB))
			{
				auto factorymidScript = objA->GetScript<FactoryMidScript>();
				factorymidScript->CheckBulletHits(objB);
			}
		}
	}
}

void CollisionManager::CheckRayToCrab()
{
	for (const RayInfo& ray : _rays)
	{
		for (auto& [crab, type] : _colliders)
		{
			if (type != COLLISION_OBJECT_TYPE::CRAB || !crab->IsEnabled())
				continue;

			const auto& obj = crab->GetGameObject();
			if (!obj)
				continue;

			if (GET_SINGLE(SceneManager)->RayCast(ray.origin, ray.direction, obj))
			{
				auto crabScript = obj->GetScript<CrabScript>();
				if (crabScript)
					crabScript->CheckBulletHits();
			}
		}
	}

	_rays.clear();
}