#include "pch.h"
#include "Frustum.h"
#include "Camera.h"

// ***************************************************
// 프러스텀 컬링은 아직 이상한 부분이 많아 손봐야 할듯
// ***************************************************

void Frustum::FinalUpdate()
{
	Matrix matViewInv = Camera::S_MatView.Invert();
	Matrix matProjectionInv = Camera::S_MatProjection.Invert();
	Matrix matInv = matProjectionInv * matViewInv;

	float scale = 10.0f;

	vector<Vec3> worldPos =
	{
		::XMVector3TransformCoord(Vec3(-scale, scale, 0.f), matInv),
		::XMVector3TransformCoord(Vec3(scale, scale, 0.f), matInv),
		::XMVector3TransformCoord(Vec3(scale, -scale, 0.f), matInv),
		::XMVector3TransformCoord(Vec3(-scale, -scale, 0.f), matInv),
		::XMVector3TransformCoord(Vec3(-scale, scale, scale), matInv),
		::XMVector3TransformCoord(Vec3(scale, scale, scale), matInv),
		::XMVector3TransformCoord(Vec3(scale, -scale, scale), matInv),
		::XMVector3TransformCoord(Vec3(-scale, -scale, scale), matInv)
	};

	_planes[PLANE_FRONT] = ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // CW
	_planes[PLANE_BACK] = ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]); // CCW
	_planes[PLANE_UP] = ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]); // CW
	_planes[PLANE_DOWN] = ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]); // CCW
	_planes[PLANE_LEFT] = ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]); // CW
	_planes[PLANE_RIGHT] = ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // CCW
}

bool Frustum::ContainsSphere(const Vec3& pos, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		const Vec4& plane = _planes[i];
		Vec3 normal = Vec3(plane.x, plane.y, plane.z);
		float distance = normal.Dot(pos) + plane.w;

		// near/far 평면에 대해서는 더 큰 여유를 줌
		float checkRadius = (i <= 1) ? radius * 3.0f : radius * 2.0f;

		// distance가 양수면 평면의 앞쪽, 음수면 뒤쪽
		if (distance > checkRadius)
		{
			// Near plane의 경우 특별 처리
			if (i == 0)
				continue;  // near plane은 무시
			return false;
		}
	}
	return true;
}