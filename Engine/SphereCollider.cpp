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
	shared_ptr<GameObject> obj{};
	obj = make_shared<GameObject>();
	obj->SetName(L"SphereCollider");
	// �������� �ø� ����
	obj->SetCheckFrustum(true);
	// ����, ���� ����
	obj->SetStatic(false);

	// 2. Transform ������Ʈ �߰� �� ����
	obj->AddComponent(make_shared<Transform>());
	obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	obj->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

	// 4. MeshRenderer ����
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
		meshRenderer->SetMesh(sphereMesh);
	}
	{
		// GameObject �̸��� ���� ���͸����� ã�� Ŭ�� �� �޽� �������� Set
		// Resources.cpp�� ���� ���̴��� ���͸����� ������ Ȯ���غ� �� ����
		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Debug");
		meshRenderer->SetMaterial(material->Clone());
	}
	obj->AddComponent(meshRenderer);

	// 5. Scene�� �߰�
	//GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(obj);
}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::FinalUpdate()
{
	_boundingSphere.Center = GetGameObject()->GetTransform()->GetLocalPosition();

	Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
	_boundingSphere.Radius = _radius * max(max(scale.x, scale.y), scale.z);
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);
}