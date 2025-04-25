#pragma once
#include "BaseCollider.h"

class BoxCollider : public BaseCollider
{
public:
	BoxCollider();
	virtual ~BoxCollider();

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
	virtual bool Intersects(shared_ptr<BaseCollider> otherCollider) override;

	void SetExtents(Vec3 extents) { _extents = extents; }
	void SetCenter(Vec3 center) { _center = center; }

	Vec3 GetExtents() const { return _extents; } 
	virtual Vec3 GetCenter() const override { return _center; }

	virtual Vec3 GetMin() override { return _center - _extents; }
	virtual Vec3 GetMax() override { return _center + _extents; }

	BoundingBox GetBoundingBox() { return _boundingBox; }

private:
	// Local ±‚¡ÿ
	Vec3		_extents = Vec3(0, 0, 0);
	Vec3		_center = Vec3(0, 0, 0);

	BoundingBox _boundingBox;
};

