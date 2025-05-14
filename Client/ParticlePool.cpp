#include "pch.h"
#include "ParticlePool.h"

void ParticlePool::Init(const vector<shared_ptr<GameObject>>& particles)
{
	_particles = particles;
}

shared_ptr<ParticleSystem> ParticlePool::GetAvailable()
{
	for (auto& obj : _particles)
	{
		auto ps = dynamic_pointer_cast<ParticleSystem>(obj->GetParticleSystem());
		if (ps && !ps->IsActive())
			return ps;
	}
	return nullptr;
}