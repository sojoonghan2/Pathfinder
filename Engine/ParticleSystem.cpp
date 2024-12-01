#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"

ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	_particleBuffer = make_shared<StructuredBuffer>();
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);

	_computeSharedBuffer = make_shared<StructuredBuffer>();
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	_mesh = GET_SINGLE(Resources)->LoadPointMesh();
	_material = GET_SINGLE(Resources)->Get<Material>(L"Particle");
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Bubbles", L"..\\Resources\\Texture\\Particle\\bubble.png");

	_material->SetTexture(0, texture);

	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle");
}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::FinalUpdate()
{
	_accTime += DELTA_TIME;

	int32 add = 0;
	if (_createInterval < _accTime)
	{
		_accTime = _accTime - _createInterval;
		add = 1;
	}

	// 파티클 버퍼와 컴퓨트 셰이더 버퍼를 UAV 슬롯에 바인딩
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
	GetTransform()->PushData();

	// 파티클 데이터를 그래픽스 셰이더에 읽기 위해 t9에 바인딩
	_particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
	_material->SetFloat(0, _startScale);
	_material->SetFloat(1, _endScale);
	_material->PushGraphicsData();

	_mesh->Render(_maxParticle);
}
