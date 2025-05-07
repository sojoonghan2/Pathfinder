#include "pch.h"
#include "CircleCollider.h"
#include "AABBCollider.h"
#include "SweptCollider.h"

bool CircleCollider::CheckCollisionWithAABB(const AABBCollider& other) const
{
	return other.CheckCollisionWithCircle(*this);
}

bool CircleCollider::CheckCollisionWithCircle(const CircleCollider& other) const
{
	Vec2f diff{ center - other.center };
	float distance{ diff.x * diff.x + diff.y * diff.y };
	float radius_sum{ radius + other.radius };
	return distance <= radius_sum * radius_sum;
}

bool CircleCollider::CheckCollisionWithSwept(const SweptCollider& other) const
{
	Vec2f line_start{ other._prevPos };
	Vec2f line_end{ other._pos };

	Vec2f d{ line_end - line_start };
	Vec2f f{ line_start - center };

	float a{ d.Dot(d) };
	float b{ 2.0f * f.Dot(d) };
	float c{ f.Dot(f) - radius * radius };

	float discriminant{ b * b - 4 * a * c };

	if (discriminant < 0.0f) {
		return false;
	}

	discriminant = std::sqrt(discriminant);

	float t1{ (-b - discriminant) / (2 * a) };
	float t2{ (-b + discriminant) / (2 * a) };

	if ((t1 >= 0.0f && t1 <= 1.0f) || (t2 >= 0.0f && t2 <= 1.0f)) {
		return true; // 선분의 범위 내에서 교차
	}

	return false;
 
}

