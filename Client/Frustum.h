#pragma once

enum PLANE_TYPE : uint8
{
	PLANE_FRONT,
	PLANE_BACK,
	PLANE_UP,
	PLANE_DOWN,
	PLANE_LEFT,
	PLANE_RIGHT,

	PLANE_END
};

// �ø��� ���� �������� Ŭ����
// �츮 ������ �ø��� �������� ������ ���������� ���� �߰�
class Frustum
{
public:
	void FinalUpdate();
	bool ContainsSphere(const Vec3& pos, float radius);

private:
	array<Vec4, PLANE_END> _planes;
};
