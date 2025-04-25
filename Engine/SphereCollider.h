#pragma once
#include "BaseCollider.h"

class SphereCollider : public BaseCollider
{
public:
	SphereCollider();
	virtual ~SphereCollider();

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
	virtual bool Intersects(shared_ptr<BaseCollider> otherCollider) override;

	void SetRadius(float radius) { _radius = radius; }
	void SetCenter(Vec3 center) { _center = center; }

	float GetRadius() const { return _boundingSphere.Radius; }
	virtual Vec3 GetCenter() const override { return _boundingSphere.Center; }

	BoundingSphere GetBoundingSphere() { return _boundingSphere; }

	virtual Vec3 GetMin() override { return _center - Vec3(_radius, _radius, _radius); }
	virtual Vec3 GetMax() override { return _center + Vec3(_radius, _radius, _radius); }

private:
	// Local ����
	float		_radius = 1.f;
	Vec3		_center = Vec3(0, 0, 0);

	BoundingSphere _boundingSphere;
};
