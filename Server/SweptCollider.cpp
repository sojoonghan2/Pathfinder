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
	// �Ѿ˰� �Ѿ� ������ �浹�� ���� ������ �Ѵ�.
	return false;
}
