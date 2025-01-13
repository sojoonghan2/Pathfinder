#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"

ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	// GPU·Î µ¥ÀÌÅÍ¸¦ Àü´ÞÇÏ±â À§ÇØ ÇÊ¿äÇÑ ¹öÆÛ¸¦ »ý¼º
	_particleBuffer = make_shared<StructuredBuffer>();
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);

	_computeSharedBuffer = make_shared<StructuredBuffer>();
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	_mesh = GET_SINGLE(Resources)->LoadPointMesh();
}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::FinalUpdate()
{
	_totalTime += DELTA_TIME;

	// »õ ÆÄÆ¼Å¬ »ý¼º Áß´Ü Á¶°Ç
	int32 add = 0;
	if (_isActive && _duration > 0.0f && _totalTime >= _duration)
	{
		_isActive = false; // »õ ÆÄÆ¼Å¬ »ý¼º Áß´Ü
	}

	// »õ ÆÄÆ¼Å¬ »ý¼º
	if (_isActive)
	{
		_accTime += DELTA_TIME;

		if (_createInterval < _accTime)
		{
			_accTime -= _createInterval;
<<<<<<< HEAD
			// ÆÄÆ¼Å¬À» 10°³¾¿ »ý¼º
			add = _onceParticleNum;
=======
			add = 1; // »õ ÆÄÆ¼Å¬ »ý¼º
>>>>>>> parent of cbe179b (ëŒ€ê²©ë³€ íŒŒí‹°í´ ì¶”ê°€)
		}
	}

	// ÄÄÇ»Æ® ¼ÎÀÌ´õ·Î µ¥ÀÌÅÍ Àü´Þ
	_particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
	_computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

	_computeMaterial->SetInt(0, _maxParticle);
	_computeMaterial->SetInt(1, add);
	_computeMaterial->SetVec2(1, Vec2(DELTA_TIME, _accTime));
	_computeMaterial->SetVec4(0, Vec4(_minLifeTime, _maxLifeTime, _minSpeed, _maxSpeed));
	_computeMaterial->Dispatch(1, 1, 1);
}

void ParticleSystem::Render()
{
	// ¸ðµç ÆÄÆ¼Å¬ÀÌ Á¾·áµÈ °æ¿ì ·»´õ¸µ Áß´Ü
	if (_totalTime >= (_duration + _maxLifeTime))
		return;

	GetTransform()->PushData();

	// ±×·¡ÇÈ½º ¼ÎÀÌ´õ·Î µ¥ÀÌÅÍ Àü´Þ
	_particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
	_material->SetFloat(0, _startScale);
	_material->SetFloat(1, _endScale);
	_material->PushGraphicsData();

	_mesh->Render(_maxParticle);
}

void ParticleSystem::SetParticleInfo(float createInterval, float accTime, float minLifeTime, float maxLifeTime, float minSpeed, float maxSpeed, float startScale, float endScale)
{
	// ÆÄÆ¼Å¬ »ý¼º °£°Ý
	_createInterval = createInterval;
	// ´©Àû ½Ã°£
	_accTime = accTime;

	// ÃÖ¼Ò ¼ö¸í
	_minLifeTime = minLifeTime;
	// ÃÖ´ë ¼ö¸í
	_maxLifeTime = maxLifeTime;

	// ÃÖ¼Ò ¼Óµµ
	_minSpeed = minSpeed;
	// ÃÖ´ë ¼Óµµ
	_maxSpeed = maxSpeed;

	// ½ÃÀÛ Å©±â
	_startScale = startScale;
	// Á¾·á Å©±â
	_endScale = endScale;
}

void ParticleSystem::SetParticleInterval(float createInterval, float accTime)
{
	// ÆÄÆ¼Å¬ »ý¼º °£°Ý
	_createInterval = createInterval;
	// ´©Àû ½Ã°£
	_accTime = accTime;
}

void ParticleSystem::SetParticleLifeTime(float minLifeTime, float maxLifeTime)
{
	// ÃÖ¼Ò ¼ö¸í
	_minLifeTime = minLifeTime;
	// ÃÖ´ë ¼ö¸í
	_maxLifeTime = maxLifeTime;
}

void ParticleSystem::SetParticleSpeed(float minSpeed, float maxSpeed)
{
	// ÃÖ¼Ò ¼Óµµ
	_minSpeed = minSpeed;
	// ÃÖ´ë ¼Óµµ
	_maxSpeed = maxSpeed;
}

void ParticleSystem::SetParticleScale(float startScale, float endScale)
{
	// ½ÃÀÛ Å©±â
	_startScale = startScale;
	// Á¾·á Å©±â
	_endScale = endScale;
}

void ParticleSystem::SetParticleTexture(shared_ptr<Texture> texture)
{
	_material->SetTexture(0, texture);
}

void ParticleSystem::SetMaterial(shared_ptr<Material> material, shared_ptr<Material> computeMaterial)
{
	_material = material;
	_computeMaterial = computeMaterial;
}

void ParticleSystem::ParticleStart()
{
	_isActive = true;

	// ÀüÃ¼ ½Ã°£ ÃÊ±âÈ­
	_totalTime = 0.0f;

	// ÃÊ±âÈ­µÈ »óÅÂ·Î ½ÃÀÛ
	_accTime = 0.0f;

	// ÇÊ¿äÇÑ °æ¿ì Ãß°¡ ÃÊ±âÈ­
	vector<ParticleInfo> emptyParticles(_maxParticle);
	// GPU·Î ÆÄÆ¼Å¬ ¹öÆÛ Àü´Þ
	_particleBuffer->Update(emptyParticles.data(), emptyParticles.size() * sizeof(ParticleInfo));
}

void ParticleSystem::ParticleStop()
{
	_isActive = false;
}

void ParticleSystem::ParticleToggle()
{
	if (_isActive) ParticleStop();
	else ParticleStart();
}
