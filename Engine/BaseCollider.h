#pragma once
#include "Component.h"

enum class ColliderType
{
	Sphere,
	Rectangle,
};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;

private:
	ColliderType _colliderType = {};
};