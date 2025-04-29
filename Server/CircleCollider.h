#pragma once
#include "Collider.h"

class CircleCollider : public Collider {
public:
	Vec2f center;
	float radius;

	bool CheckCollision(const std::shared_ptr<const Collider>& other) const override {
		return other->CheckCollisionWithCircle(*this);
	}

	bool CheckCollisionWithCircle(const CircleCollider& other) const override;

	bool CheckCollisionWithAABB(const AABBCollider& other) const override;
};