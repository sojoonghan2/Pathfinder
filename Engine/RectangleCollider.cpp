#include "pch.h"
#include "RectangleCollider.h"
#include "GameObject.h"
#include "Transform.h"

RectangleCollider::RectangleCollider() : BaseCollider(ColliderType::Rectangle)
{
}

RectangleCollider::~RectangleCollider()
{
}

void RectangleCollider::FinalUpdate()
{
    Vec3 worldCenter = GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();

    // AABB 업데이트
    _boundingBox.Center = worldCenter;
    _boundingBox.Extents = scale;
}

bool RectangleCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}
