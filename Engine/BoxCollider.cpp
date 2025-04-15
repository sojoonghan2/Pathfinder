#include "pch.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"

BoxCollider::BoxCollider() : BaseCollider(ColliderType::Box)
{

}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::FinalUpdate()
{
    Vec3 worldPos = GetTransform()->GetWorldPosition();

    _boundingBox.Center = worldPos + _center;
    _boundingBox.Extents = _extents;
}

bool BoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}

bool BoxCollider::Intersects(shared_ptr<BaseCollider> otherCollider)
{
    if (auto otherBox = dynamic_pointer_cast<BoxCollider>(otherCollider))
    {
        Vec3 worldCenterA = GetGameObject()->GetTransform()->GetWorldPosition() + _center;
        Vec3 minA = worldCenterA - _extents;
        Vec3 maxA = worldCenterA + _extents;

        Vec3 worldCenterB = otherBox->GetGameObject()->GetTransform()->GetWorldPosition() + otherBox->GetCenter();
        Vec3 minB = worldCenterB - otherBox->GetExtents();
        Vec3 maxB = worldCenterB + otherBox->GetExtents();

        return (minA.x <= maxB.x && maxA.x >= minB.x &&
            minA.y <= maxB.y && maxA.y >= minB.y &&
            minA.z <= maxB.z && maxA.z >= minB.z);
    }

    if (auto otherSphere = dynamic_pointer_cast<SphereCollider>(otherCollider))
    {
        Vec3 sphereCenter = otherSphere->GetGameObject()->GetTransform()->GetWorldPosition() + otherSphere->GetCenter();
        float sphereRadius = otherSphere->GetRadius();

        Vec3 worldCenter = GetGameObject()->GetTransform()->GetWorldPosition() + _center;
        Vec3 min = worldCenter - _extents;
        Vec3 max = worldCenter + _extents;

        Vec3 closestPoint = sphereCenter;

        closestPoint.x = std::max<float>(min.x, std::min<float>(sphereCenter.x, max.x));
        closestPoint.y = std::max<float>(min.y, std::min<float>(sphereCenter.y, max.y));
        closestPoint.z = std::max<float>(min.z, std::min<float>(sphereCenter.z, max.z));

        Vec3 distanceVec = closestPoint - sphereCenter;
        return distanceVec.LengthSquared() <= (sphereRadius * sphereRadius);
    }

    return false;
}