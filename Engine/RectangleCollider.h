#pragma once
#include "BaseCollider.h"

class RectangleCollider : public BaseCollider
{
public:
	RectangleCollider();
	virtual ~RectangleCollider();

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;

	void SetSize(Vec3 size) { _size = size; }
	void SetCenter(Vec3 center) { _center = center; }

private:
	// AABB (�� ���� �ٿ�� �ڽ�)
	BoundingBox _boundingBox;

	// ���� ����
	Vec3 _size = Vec3(10.f, 50.f, 10.f);
	Vec3 _center = Vec3(0, 0, 0);
};
