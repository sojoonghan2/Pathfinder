#pragma once
#include "MonoBehaviour.h"

class GeneratorScript : public MonoBehaviour
{
public:
	GeneratorScript();

	virtual void Start() override;
	virtual void LateUpdate() override;

	void CheckBulletHits(shared_ptr<GameObject> bullet);
	void Recovery();

private:
	shared_ptr<GameObject>			_hp;
	shared_ptr<GameObject>			_generatorParticle;

	bool							_isDead = false;
	bool							_init = false;
	bool							_initialized = false;

	float							_deadTime;

	float							_hitTime = 100.f;
	float							_hitDuration = 0.2f;

	float							_recoveryWaitTime = 5.f;
	float							_recoveryElapsedTime = 0.f;
	float							_hpMax = 480.f;
	bool							_wasHit = false;

};
