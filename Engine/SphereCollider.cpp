#include "pch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
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
    // �ٸ� Collider�� SphereCollider�� ���
    if (auto otherSphere = dynamic_pointer_cast<SphereCollider>(otherCollider))
    {
        float distance = (GetCenter() - otherSphere->GetCenter()).Length();
        return distance < (GetRadius() + otherSphere->GetRadius());
    }

    // �ٸ� Collider�� BoxCollider�� ���
    if (auto otherBox = dynamic_pointer_cast<BoxCollider>(otherCollider))
    {
        Vec3 closestPoint = GetCenter();

        // Box�� ��迡 ���� ����� �� ã��
        closestPoint.x = std::max<float>(otherBox->GetMin().x, std::min<float>(GetCenter().x, otherBox->GetMax().x));
        closestPoint.y = std::max<float>(otherBox->GetMin().y, std::min<float>(GetCenter().y, otherBox->GetMax().y));
        closestPoint.z = std::max<float>(otherBox->GetMin().z, std::min<float>(GetCenter().z, otherBox->GetMax().z));

        // �Ÿ� ���
        Vec3 distanceVec = closestPoint - GetCenter();
        return distanceVec.LengthSquared() <= (GetRadius() * GetRadius());
    }

    return false;
}