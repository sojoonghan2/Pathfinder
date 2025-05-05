#pragma once
#include "GameObject.h"
#include "ParticleSystem.h"

class ParticlePool
{
public:
	void Init(const vector<shared_ptr<GameObject>>& particles);

	shared_ptr<ParticleSystem> GetAvailable();

private:
	vector<shared_ptr<GameObject>> _particles;
};

