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

class Frustum
{
public:
	void FinalUpdate(Matrix matV, Matrix matP);
	bool ContainsSphere(const Vec3& pos, float radius);
	bool ContainsSphereShadow(const Vec3& pos, float radius);
	const vector<Vec3>& GetFrustum() { return m_frustum; };

private:
	array<Vec4, PLANE_END> _planes;
	vector<Vec3> m_frustum;

};

