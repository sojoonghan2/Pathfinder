#pragma once
#include "MonoBehaviour.h"
#include "ParticlePool.h"

class NetworkCrabScript : public MonoBehaviour
{
public:
	NetworkCrabScript();
	virtual void Start() override;
	virtual void LateUpdate() override;
	virtual void Awake() override;

	void SetPosition(float x, float z);
	void SetDir(float x, float z);

	void Animation();

	void RegisterParticles(const vector<shared_ptr<GameObject>>& particles) { _particleObjects = particles; }

private:

	float		_lastX{ -1.f };
	float		_lastY{ -1.f };
	bool		_isMove{ false };
	uint32		_currentAnimIndex{ 0 };

	float		_currentHp{ 0.f };

	vector<shared_ptr<GameObject>> _particleObjects;
	ParticlePool _particlePool;
};
