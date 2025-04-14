#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "DebugRenderer.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "SceneManager.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere)
{

}

SphereCollider::~SphereCollider()
{

}	

void SphereCollider::FinalUpdate()
{
	_boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition();

	_boundingSphere.Radius = _radius;
	_boundingSphere.Center = GetCenter();

	//DebugRenderer::DrawSphere(_boundingSphere.Center, _boundingSphere.Radius, Vec4(1, 0, 0, 1));
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);
}

bool SphereCollider::Intersects(shared_ptr<BaseCollider> otherCollider)
{
	if (auto otherSphere = dynamic_pointer_cast<SphereCollider>(otherCollider))
	{
		Vec3 centerA = GetCenter();
		Vec3 centerB = otherSphere->GetCenter();

		float distance = (centerA - centerB).Length();
		float sumRadius = GetRadius() + otherSphere->GetRadius();

		return distance < sumRadius;
	}
	return false;
}