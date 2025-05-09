#include "pch.h"
#include "SweptCollider.h"
#include "AABBCollider.h"
#include "CircleCollider.h"

bool SweptCollider::CheckCollisionWithAABB(const AABBCollider& other) const
{
	return other.CheckCollisionWithSwept(*this);
}

bool SweptCollider::CheckCollisionWithCircle(const CircleCollider& other) const
{
	return other.CheckCollisionWithSwept(*this);
}

bool SweptCollider::CheckCollisionWithSwept(const SweptCollider& other) const
{
	// 총알과 총알 사이의 충돌은 없는 것으로 한다.
	return false;
}
