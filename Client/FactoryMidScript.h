#pragma once
#include "MonoBehaviour.h"
#include "ParticlePool.h"
#include "GameObject.h"

class FactoryMidScript : public MonoBehaviour
{
public:
	virtual void Start() override;
	virtual void LateUpdate() override;

	void RegisterParticles(const vector<shared_ptr<GameObject>>& particles) { _particleObjects = particles; }
	void CheckBulletHits(shared_ptr<GameObject> bullet);

private:
	vector<shared_ptr<GameObject>>	_particleObjects;
	ParticlePool					_particlePool;

	float							_rotationSpeed = 0.5f;
};
