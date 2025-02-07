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
    Vec3 halfSize = (_size * scale) * 0.5f;

    // AABB 업데이트
    _boundingBox.Center = worldCenter;
    _boundingBox.Extents = halfSize;
}

bool RectangleCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}
