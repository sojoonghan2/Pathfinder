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
	// AABB (축 정렬 바운딩 박스)
	BoundingBox _boundingBox;

	// 로컬 기준
	Vec3 _size = Vec3(300.f, 500.f, 100.f);
	Vec3 _center = Vec3(0, 0, 0);
};
