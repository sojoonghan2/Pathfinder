#pragma once

enum class ColliderType {
	None,
	Circle,
	AABB,
};




class Collider
{
public:

	virtual ColliderType GetColliderType() const = 0;
	virtual bool CheckCollision(const Collider& other) const = 0;

	Collider() = default;

	// 복사 금지
	Collider(const Collider& other) = delete;
	Collider& operator=(const Collider& other) = delete;

private:


};

