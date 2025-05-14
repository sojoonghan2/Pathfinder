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

// 컬링을 위한 프러스텀 클래스
// 우리 게임은 컬링을 지원하지 않지만 유지보수를 위해 추가
class Frustum
{
public:
	void FinalUpdate();
	bool ContainsSphere(const Vec3& pos, float radius);

private:
	array<Vec4, PLANE_END> _planes;
};
