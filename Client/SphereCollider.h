#pragma once
#include "BaseCollider.h"

class OrientedBoxCollider;

class SphereCollider : public BaseCollider
{
public:
	SphereCollider();
	virtual ~SphereCollider();

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
	virtual bool Intersects(shared_ptr<BaseCollider> otherCollider) override;
	virtual bool Intersects(shared_ptr<BoundingSphere> boundingSphere) override;
	virtual bool Intersects(shared_ptr<BoundingBox> boundingBox) override;
	virtual bool Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox) override;
	virtual bool Intersects(shared_ptr<SphereCollider> other);
	virtual bool Intersects(shared_ptr<OrientedBoxCollider> other);

	shared_ptr<BoundingSphere> GetBoundingSphere() { return _boundingSphere; }

private:
	shared_ptr<BoundingSphere> _boundingSphere;
};