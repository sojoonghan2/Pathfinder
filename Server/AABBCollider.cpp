#include "pch.h"
#include "AABBCollider.h"
#include "CircleCollider.h"
#include "SweptCollider.h"


bool AABBCollider::CheckCollision(const std::shared_ptr<const Collider>& other) const
{
	return other->CheckCollisionWithAABB(*this);
}

bool AABBCollider::CheckCollisionWithAABB(const AABBCollider& other) const
{
	Vec2f a_min{ GetMin() };
	Vec2f a_max{ GetMax() };
	Vec2f b_min{ other.GetMin() };
	Vec2f b_max{ other.GetMax() };

	return (a_min.x <= b_max.x && a_max.x >= b_min.x) &&
		(a_min.y <= b_max.y && a_max.y >= b_min.y);
}

bool AABBCollider::CheckCollisionWithCircle(const CircleCollider& other) const
{

	Vec2f min{ GetMin() };
	Vec2f max{ GetMax() };

	float closest_x = std::max(min.x, std::min(other.center.x, max.x));
	float closest_y = std::max(min.y, std::min(other.center.y, max.y));

	Vec2f diff = other.center - Vec2f{closest_x, closest_y};
	return diff.x * diff.x + diff.y * diff.y <= other.radius * other.radius;
}

bool AABBCollider::CheckCollisionWithSwept(const SweptCollider& other) const
{
	return false;
}
