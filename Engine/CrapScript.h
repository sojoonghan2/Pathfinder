#pragma once
#include "MonoBehaviour.h"

class CrapScript : public MonoBehaviour
{
public:
	CrapScript();
	virtual void LateUpdate() override;

	void MoveRandomly();
	void CheckBoundary();

private:
	float _speed = 1000.f;
	Vec3 _direction;
	float _changeDirectionTime = 10.0f;
	float _elapsedTime = 0.0f;

	bool _isMoving = false;
	float _pauseDuration{};
};
