#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"

ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	// GPU로 데이터를 전달하기 위해 필요한 버퍼를 생성
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

	// 새 파티클 생성 중단 조건
	int32 add = 0;
	if (_isActive && _duration > 0.0f && _totalTime >= _duration)
	{
		_isActive = false; // 새 파티클 생성 중단
	}

	// 새 파티클 생성
	if (_isActive)
	{
		_accTime += DELTA_TIME;

		if (_createInterval < _accTime)
		{
			_accTime -= _createInterval;
			// 파티클을 10개씩 생성
			add = _onceParticleNum;
			add = 1; // 새 파티클 생성
		}
	}

	// 컴퓨트 셰이더로 데이터 전달
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
	// 모든 파티클이 종료된 경우 렌더링 중단
	if (_totalTime >= (_duration + _maxLifeTime))
		return;

	GetTransform()->PushData();

	// 그래픽스 셰이더로 데이터 전달
	_particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
	_material->SetFloat(0, _startScale);
	_material->SetFloat(1, _endScale);
	_material->PushGraphicsData();

	_mesh->Render(_maxParticle);
}

void ParticleSystem::SetParticleInfo(float createInterval, float accTime, float minLifeTime, float maxLifeTime, float minSpeed, float maxSpeed, float startScale, float endScale)
{
	// 파티클 생성 간격
	_createInterval = createInterval;
	// 누적 시간
	_accTime = accTime;

	// 최소 수명
	_minLifeTime = minLifeTime;
	// 최대 수명
	_maxLifeTime = maxLifeTime;

	// 최소 속도
	_minSpeed = minSpeed;
	// 최대 속도
	_maxSpeed = maxSpeed;

	// 시작 크기
	_startScale = startScale;
	// 종료 크기
	_endScale = endScale;
}

void ParticleSystem::SetParticleInterval(float createInterval, float accTime)
{
	// 파티클 생성 간격
	_createInterval = createInterval;
	// 누적 시간
	_accTime = accTime;
}

void ParticleSystem::SetParticleLifeTime(float minLifeTime, float maxLifeTime)
{
	// 최소 수명
	_minLifeTime = minLifeTime;
	// 최대 수명
	_maxLifeTime = maxLifeTime;
}

void ParticleSystem::SetParticleSpeed(float minSpeed, float maxSpeed)
{
	// 최소 속도
	_minSpeed = minSpeed;
	// 최대 속도
	_maxSpeed = maxSpeed;
}

void ParticleSystem::SetParticleScale(float startScale, float endScale)
{
	// 시작 크기
	_startScale = startScale;
	// 종료 크기
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

	// 전체 시간 초기화
	_totalTime = 0.0f;

	// 초기화된 상태로 시작
	_accTime = 0.0f;

	// 필요한 경우 추가 초기화
	vector<ParticleInfo> emptyParticles(_maxParticle);
	// GPU로 파티클 버퍼 전달
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
