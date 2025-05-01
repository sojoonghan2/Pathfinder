#pragma once
#include "Collider.h"

// �Ѿ˰� ���� ���� ��ü�� �� collider
// ���� ��ġ�� ����ؼ� ���� ��ġ�� ���� ��ġ�� ���� �׾� �� �������� �浹 ó���� �Ѵ�
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

