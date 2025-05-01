#pragma once
#include "Collider.h"

// 총알과 같은 빠른 객체에 들어갈 collider
// 이전 위치를 기록해서 현재 위치와 이전 위치의 선을 그어 그 선분으로 충돌 처리를 한다
class SweptCollider : public Collider
{
	bool CheckCollision(const std::shared_ptr<const Collider>& other) const override {
		return other->CheckCollisionWithCircle(*this);
	}
	bool CheckCollisionWithAABB(const AABBCollider& other) const override;
	bool CheckCollisionWithCircle(const CircleCollider& other) const override;
	bool CheckCollisionWithSwept(const SweptCollider& other) const override;


public:

};

