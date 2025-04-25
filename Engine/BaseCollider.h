#pragma once
#include "Component.h"

enum class ColliderType
{
	Sphere,
	Box,
	Rectangle,
};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;
	virtual bool Intersects(shared_ptr<BaseCollider> otherCollider) = 0;

	virtual Vec3 GetMin() = 0;
	virtual Vec3 GetMax() = 0;

	virtual Vec3 GetCenter() const = 0;

private:
	ColliderType _colliderType = {};
};