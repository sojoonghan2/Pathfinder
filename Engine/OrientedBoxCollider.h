#pragma once
#include "BaseCollider.h"

class SphereCollider;

class OrientedBoxCollider : public BaseCollider
{
public:
	OrientedBoxCollider();
	virtual ~OrientedBoxCollider();

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance);
	virtual bool Intersects(shared_ptr<BaseCollider> otherCollider) override;
	virtual bool Intersects(shared_ptr<BoundingSphere> boundingSphere) override;
	virtual bool Intersects(shared_ptr<BoundingBox> boundingBox) override;
	virtual bool Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox) override;
	virtual bool Intersects(shared_ptr<OrientedBoxCollider> other);
	virtual bool Intersects(shared_ptr<SphereCollider> other);


	shared_ptr<BoundingOrientedBox> GetBoundingOrienteBox() { return _boundingOrientedBox; }

private:
	shared_ptr<BoundingOrientedBox> _boundingOrientedBox;
};

