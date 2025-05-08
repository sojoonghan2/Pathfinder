#pragma once
#include "Component.h"

enum class ColliderType
{
	Sphere,
	OrientedBox,
	Rectangle,
};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;
	virtual bool Intersects(shared_ptr<BaseCollider> otherCollider) = 0;
	virtual bool Intersects(shared_ptr<BoundingSphere> boundingSphere) = 0;
	virtual bool Intersects(shared_ptr<BoundingBox> boundingBox) = 0;
	virtual bool Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox) = 0;

	void SetRadius(float radius) { _radius = radius; }
	void SetCenter(Vec3 center) { _center = center; }
	void SetExtents(Vec3 extents) { _extents = extents; }
	void SetRotation(Vec3 rotation) { _rotation = rotation; }

	shared_ptr<GameObject> GetDebugCollider() { return _debugCollider; }

	void SetEnable(bool able) { _enable = able; }
	bool IsEnabled() { return _enable; }

protected:
	bool						_enable = true;
	Vec3						_center = {}; // �ݶ��̴��� ���� �߽� ��ġ
	float						_radius = {}; // �ݶ��̴� ������
	Vec3						_extents = {}; // �ݶ��̴� ũ��
	Vec3						_rotation = { 0.f, 0.f, 0.f }; // �ݶ��̴� ȸ��

	Matrix						_matLocal;
	Matrix						_matWorld;

	shared_ptr<GameObject>		_debugCollider;

private:
	ColliderType _colliderType = {};
};