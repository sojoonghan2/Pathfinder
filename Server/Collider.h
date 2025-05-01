#pragma once

enum class ColliderType {
	None,
	AABB,
	Circle,
	Swept,
};



// use double dispatcher
class Collider
{
public:

	virtual ColliderType GetColliderType() const = 0;
	virtual bool CheckCollision(const std::shared_ptr<const Collider>& other) const = 0;
	virtual bool CheckCollisionWithAABB(const class AABBCollider& other) const = 0;
	virtual bool CheckCollisionWithCircle(const class CircleCollider& other) const = 0;
	virtual bool CheckCollisionWithSwept(const class SweptCollider& other) const = 0;

	Collider() = default;

	// 복사 금지
	Collider(const Collider& other) = delete;
	Collider& operator=(const Collider& other) = delete;

private:


};

