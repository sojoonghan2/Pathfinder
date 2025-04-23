#pragma once
#include "MonoBehaviour.h"

class GunScript : public MonoBehaviour
{
public:
	GunScript();
	virtual ~GunScript();

	virtual void Update() override;

private:
	void UpdateGunTransformByInput();
	void ApplyTransformInstantly();

	void FlamePlaying();

	void SetIdlePose();
	void SetMovePose();
	void SetDashPose();
	void SetGrenadePose();
	void SetShootIdlePose();
	void SetShootMovePose();
	void SetRazerPose();

private:
	std::unordered_map<int, std::pair<Vec3, Vec3>> _gunTransforms;

	Vec3 _targetPos = Vec3(0.f);
	Vec3 _targetRot = Vec3(0.f);
	float _idleTime = 0.f;
	float _shootMoveTime = 0.f;

	bool _isFlamePlaying = false;
};