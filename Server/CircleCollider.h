#pragma once
#include "Collider.h"

class CircleCollider : public Collider {
public:
	ColliderType GetColliderType() const override { return ColliderType::Circle; }

	bool CheckCollision(const std::shared_ptr<const Collider>& other) const override {
		return other->CheckCollisionWithCircle(*this);
	}

	bool CheckCollisionWithAABB(const AABBCollider& other) const override;
	bool CheckCollisionWithCircle(const CircleCollider& other) const override;
	bool CheckCollisionWithSwept(const SweptCollider& other) const override;

	CircleCollider() = delete;
	CircleCollider(const Vec2f& center, const float radius) :
		center{ center },
		radius{ radius }
	{}

public:
	const Vec2f& center;
	float radius;

};