#include "pch.h"
#include "AABBCollider.h"

bool AABBCollider::CheckCollision(const Collider& other) const
{
	switch (other.GetColliderType())
	{
	case ColliderType::AABB:
	{
		auto& AABB_other{ static_cast<const AABBCollider&>(other) };
		Vec2f a_min{ GetMin() };
		Vec2f a_max{ GetMax() };
		Vec2f b_min{ AABB_other.GetMin() };
		Vec2f b_max{ AABB_other.GetMax() };

		return (a_min.x <= b_max.x && a_max.x >= b_min.x) &&
			(a_min.y <= b_max.y && a_max.y >= b_min.y);
	}
	break;
	case ColliderType::Circle:
	{

		return false;
	}
	break;
	default:
	break;
	}

	return false;
}
