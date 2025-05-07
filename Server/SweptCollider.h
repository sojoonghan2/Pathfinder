#pragma once
#include "Collider.h"

// �Ѿ˰� ���� ���� ��ü�� �� collider
// ���� ��ġ�� ����ؼ� ���� ��ġ�� ���� ��ġ�� ���� �׾� �� �������� �浹 ó���� �Ѵ�
class SweptCollider : public Collider
{
public:
	bool CheckCollision(const std::shared_ptr<const Collider>& other) const override {
		return other->CheckCollisionWithSwept(*this);
	}
	bool CheckCollisionWithAABB(const AABBCollider& other) const override;
	bool CheckCollisionWithCircle(const CircleCollider& other) const override;
	bool CheckCollisionWithSwept(const SweptCollider& other) const override;

	SweptCollider() = delete;
	SweptCollider(const Vec2f& pos, const Vec2f& prev_pos) :
		_pos{ pos },
		_prevPos{ prev_pos }
	{}

public:
	const Vec2f& _pos;
	Vec2f _prevPos;

};

