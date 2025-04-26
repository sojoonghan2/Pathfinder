#pragma once
#include "MonoBehaviour.h"

class CrapScript : public MonoBehaviour
{
public:
	CrapScript();
	virtual void LateUpdate() override;
	virtual void Start() override;

	void MoveRandomly();
	void CheckBoundary();
	void CheckBulletHits();

private:
	float _speed = 1000.f;
	Vec3 _direction;
	float _changeDirectionTime = 10.0f;
	float _elapsedTime = 0.0f;

	bool _isMoving = false;
	float _pauseDuration{};

	bool _initialized = false;

private:
	vector<shared_ptr<GameObject>>	_bullets;
	shared_ptr<Transform>			_hpTransform;

	int32							_index = -1;
};
