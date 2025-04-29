#include "pch.h"
#include "CircleCollider.h"
#include "AABBCollider.h"

bool CircleCollider::CheckCollisionWithCircle(const CircleCollider& other) const
{
	Vec2f diff{ center - other.center };
	float distance{ diff.x * diff.x + diff.y * diff.y };
	float radius_sum{ radius + other.radius };
	return distance <= radius_sum * radius_sum;
}

bool CircleCollider::CheckCollisionWithAABB(const AABBCollider& other) const
{
	return other.CheckCollisionWithCircle(*this);
}
