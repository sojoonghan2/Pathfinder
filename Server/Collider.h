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


private:


};

