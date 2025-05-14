#pragma once
#include "GameObject.h"
#include "ParticleSystem.h"

// 파티클을 담아두고 효율적으로 풀링
class ParticlePool
{
public:
	void Init(const vector<shared_ptr<GameObject>>& particles);

	shared_ptr<ParticleSystem> GetAvailable();

	shared_ptr<GameObject> GetNext()
	{
		if (_particles.empty()) return nullptr;

		auto obj = _particles[_index];
		_index = (_index + 1) % _particles.size();
		return obj;
	}

private:
	vector<shared_ptr<GameObject>> _particles;
	int								_index;
};

