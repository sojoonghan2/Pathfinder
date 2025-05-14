#include "pch.h"
#include "SphereCollider.h"
#include "OrientedBoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include "Input.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere)
{
	_boundingSphere = make_shared<BoundingSphere>(); // 바운딩 박스 생성

}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::FinalUpdate()
{
	Vec3 position = GetTransform()->GetWorldPosition();

	_boundingSphere->Center = _center + position;
	_boundingSphere->Radius = _radius;
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingSphere->Intersects(rayOrigin, rayDir, OUT distance);
}

bool SphereCollider::Intersects(shared_ptr<BaseCollider> otherCollider)
{
	if (auto otherBox = dynamic_pointer_cast<OrientedBoxCollider>(otherCollider))
		return Intersects(otherBox);
	if (auto otherSphere = dynamic_pointer_cast<SphereCollider>(otherCollider))
		return Intersects(otherSphere);
	return false;
}

bool SphereCollider::Intersects(shared_ptr<SphereCollider> other)
{
	return Intersects(other->GetBoundingSphere());
}

bool SphereCollider::Intersects(shared_ptr<OrientedBoxCollider> other)
{
	return Intersects(other->GetBoundingOrienteBox());
}

bool SphereCollider::Intersects(shared_ptr<BoundingSphere> boundingSphere)
{
	return _boundingSphere->Intersects(*boundingSphere);
}

bool SphereCollider::Intersects(shared_ptr<BoundingBox> boundingBox)
{
	return _boundingSphere->Intersects(*boundingBox);
}

bool SphereCollider::Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox)
{
	return _boundingSphere->Intersects(*boundingOrientedBox);
}