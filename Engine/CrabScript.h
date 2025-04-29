#pragma once
#include "MonoBehaviour.h"

class CrabScript : public MonoBehaviour
{
public:
	CrabScript();
	virtual void LateUpdate() override;
	virtual void Start() override;

	void MoveRandomly();
	void CheckBoundary();
	void CheckBulletHits();
	void CheckGrenadeHits();
	void CheckRazerHits();

	void DeadAnimation();

private:
	float _speed = 1000.f;
	Vec3 _direction;
	float _changeDirectionTime = 10.0f;
	float _elapsedTime = 0.0f;

	bool _isMoving = false;
	float _pauseDuration{};

	bool _initialized = false;

	bool _takeGrenade = false;

private:
	vector<shared_ptr<GameObject>>	_bullets;
	shared_ptr<Transform>			_hpTransform;
	shared_ptr<GameObject>			_player;
	shared_ptr<GameObject>			_grenade;

	int32							_index = -1;
	bool							_isAlive = true;
	bool							_deathHandled = false;

	Vec3							_startPos = {};
	Vec3							_targetPos = {};
	Vec3							_startRot = {};
	Vec3							_targetRot = {};
	float							_deathAnimTime = 0.f;
	bool							_isDying = false;

};
