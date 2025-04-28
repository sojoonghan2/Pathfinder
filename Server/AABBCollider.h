#pragma once
#include "collider.h"

class AABBCollider : public Collider
{
public:
	ColliderType GetColliderType() const override { return ColliderType::AABB; }
	bool CheckCollision(const Collider& other) const override;


	AABBCollider() = delete;
	AABBCollider(const Vec2f& center, const float halfWidth, const float halfHeight) :
		_center{ center },
		_width{ halfWidth },
		_height{ halfHeight }
	{}


	Vec2f GetMin() const {
		return { _center.x - _width / 2.f, _center.y - _height / 2.f };
	}

	Vec2f GetMax() const {
		return { _center.x + _width / 2.f, _center.y + _height / 2.f };
	}


private:
	// �� collider�� ������ �ִ� object�� pos�� reference�� ����.
	const Vec2f& _center;
	float _width;
	float _height;
};

