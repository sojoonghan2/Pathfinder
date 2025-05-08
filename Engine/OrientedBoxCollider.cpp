#include "pch.h"
#include "SphereCollider.h"
#include "OrientedBoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Resources.h"

OrientedBoxCollider::OrientedBoxCollider() : BaseCollider(ColliderType::OrientedBox)
{
	_boundingOrientedBox = make_shared<BoundingOrientedBox>(); // 바운딩 박스 생성
}

OrientedBoxCollider::~OrientedBoxCollider()
{

}

void OrientedBoxCollider::FinalUpdate()
{
	Vec3 position = GetTransform()->GetWorldPosition();

	_boundingOrientedBox->Center = _center + position;
	_boundingOrientedBox->Extents = _extents;
}

bool OrientedBoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingOrientedBox->Intersects(rayOrigin, rayDir, OUT distance);
}

bool OrientedBoxCollider::Intersects(shared_ptr<BaseCollider> otherCollider)
{
	if (auto otherBox = dynamic_pointer_cast<OrientedBoxCollider>(otherCollider))
		return Intersects(otherBox);
	if (auto otherSphere = dynamic_pointer_cast<SphereCollider>(otherCollider))
		return Intersects(otherSphere);
	return false;
}

bool OrientedBoxCollider::Intersects(shared_ptr<OrientedBoxCollider> other)
{
	return Intersects(other->GetBoundingOrienteBox());
}

bool OrientedBoxCollider::Intersects(shared_ptr<SphereCollider> other)
{
	return Intersects(other->GetBoundingSphere());
}

bool OrientedBoxCollider::Intersects(shared_ptr<BoundingSphere> boundingSphere)
{
	return _boundingOrientedBox->Intersects(*boundingSphere);
}

bool OrientedBoxCollider::Intersects(shared_ptr<BoundingBox> boundingBox)
{
	return _boundingOrientedBox->Intersects(*boundingBox);
}

bool OrientedBoxCollider::Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox)
{
	return _boundingOrientedBox->Intersects(*boundingOrientedBox);
}