#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"

ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	// GPU�� �����͸� �����ϱ� ���� �ʿ��� ���۸� ����
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

	// �� ��ƼŬ ���� �ߴ� ����
	int32 add = 0;
	if (_isActive && _duration > 0.0f && _totalTime >= _duration)
	{
		_isActive = false; // �� ��ƼŬ ���� �ߴ�
	}

	// �� ��ƼŬ ����
	if (_isActive)
	{
		_accTime += DELTA_TIME;

		if (_createInterval < _accTime)
		{
			_accTime -= _createInterval;
			// ��ƼŬ�� 10���� ����
			add = _onceParticleNum;
			add = 1; // �� ��ƼŬ ����
		}
	}

	// ��ǻƮ ���̴��� ������ ����
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
	// ��� ��ƼŬ�� ����� ��� ������ �ߴ�
	if (_totalTime >= (_duration + _maxLifeTime))
		return;

	GetTransform()->PushData();

	// �׷��Ƚ� ���̴��� ������ ����
	_particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
	_material->SetFloat(0, _startScale);
	_material->SetFloat(1, _endScale);
	_material->PushGraphicsData();

	_mesh->Render(_maxParticle);
}

void ParticleSystem::SetParticleInfo(float createInterval, float accTime, float minLifeTime, float maxLifeTime, float minSpeed, float maxSpeed, float startScale, float endScale)
{
	// ��ƼŬ ���� ����
	_createInterval = createInterval;
	// ���� �ð�
	_accTime = accTime;

	// �ּ� ����
	_minLifeTime = minLifeTime;
	// �ִ� ����
	_maxLifeTime = maxLifeTime;

	// �ּ� �ӵ�
	_minSpeed = minSpeed;
	// �ִ� �ӵ�
	_maxSpeed = maxSpeed;

	// ���� ũ��
	_startScale = startScale;
	// ���� ũ��
	_endScale = endScale;
}

void ParticleSystem::SetParticleInterval(float createInterval, float accTime)
{
	// ��ƼŬ ���� ����
	_createInterval = createInterval;
	// ���� �ð�
	_accTime = accTime;
}

void ParticleSystem::SetParticleLifeTime(float minLifeTime, float maxLifeTime)
{
	// �ּ� ����
	_minLifeTime = minLifeTime;
	// �ִ� ����
	_maxLifeTime = maxLifeTime;
}

void ParticleSystem::SetParticleSpeed(float minSpeed, float maxSpeed)
{
	// �ּ� �ӵ�
	_minSpeed = minSpeed;
	// �ִ� �ӵ�
	_maxSpeed = maxSpeed;
}

void ParticleSystem::SetParticleScale(float startScale, float endScale)
{
	// ���� ũ��
	_startScale = startScale;
	// ���� ũ��
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

	// ��ü �ð� �ʱ�ȭ
	_totalTime = 0.0f;

	// �ʱ�ȭ�� ���·� ����
	_accTime = 0.0f;

	// �ʿ��� ��� �߰� �ʱ�ȭ
	vector<ParticleInfo> emptyParticles(_maxParticle);
	// GPU�� ��ƼŬ ���� ����
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
