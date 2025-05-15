#pragma once
#include "MonoBehaviour.h"
#include "ParticlePool.h"
#include "GameObject.h"

class CrabScript : public MonoBehaviour
{
public:
	CrabScript();
	virtual void Start() override;
	virtual void LateUpdate() override;

	void MoveRandomly();
	void CheckBoundary();
	void CheckDummyHits(shared_ptr<GameObject> dummy);
	void CheckBulletHits();
	void CheckGrenadeHits();
	void CheckRazerHits();

	void DeadAnimation();

	void RegisterParticles(const vector<shared_ptr<GameObject>>& particles) { _particleObjects = particles; }

private:
	vector<shared_ptr<GameObject>> _particleObjects;
	ParticlePool _particlePool;

	float _speed = 1000.f;
	Vec3 _direction;
	float _changeDirectionTime = 10.0f;
	float _elapsedTime = 0.0f;

	bool _isMoving = false;
	float _pauseDuration{};

	bool _initialized = false;

	bool _takeGrenade = false;

private:
	shared_ptr<GameObject>			_hp;
	shared_ptr<GameObject>			_player;

	int32							_index = -1;
	bool							_isAlive = true;
	bool							_deathHandled = false;

	Vec3							_startPos = {};
	Vec3							_targetPos = {};
	Vec3							_startRot = {};
	Vec3							_targetRot = {};
	float							_deathAnimTime = 0.f;
	bool							_isDying = false;

	float							_hitTime = 100.f;
	float							_hitDuration = 0.2f;
};
