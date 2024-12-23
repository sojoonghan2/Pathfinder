#pragma once
#include "Component.h"

class Material;
class Mesh;
class StructuredBuffer;

struct ParticleInfo
{
	// 월드 좌표
	Vec3	worldPos;
	// 현재 경과 시간
	float	curTime;
	// 이동 방향
	Vec3	worldDir;
	// 수명
	float	lifeTime;
	// 활성 상태
	int32	alive;
	// 패딩(12 + 4 + 12 + 4 + 4 + 12(패딩))
	int32	padding[3];
};

// 컴퓨트 셰이더에서 공유 데이터를 저장하는 버퍼
struct ComputeSharedInfo
{
	int32 addCount;
	int32 padding[3];
};

class ParticleSystem : public Component
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

public:
	virtual void FinalUpdate();
	void Render();

public:
	virtual void Load(const wstring& path) override { }
	virtual void Save(const wstring& path) override { }

private:
	shared_ptr<StructuredBuffer>	_particleBuffer;
	shared_ptr<StructuredBuffer>	_computeSharedBuffer;
	uint32							_maxParticle = 1000;

	shared_ptr<Material>		_computeMaterial;
	shared_ptr<Material>		_material;
	shared_ptr<Mesh>			_mesh;

	float				_createInterval = 0.005f;
	float				_accTime = 0.f;

	float				_minLifeTime = 0.5f;
	float				_maxLifeTime = 1.f;
	float				_minSpeed = 100;
	float				_maxSpeed = 50;
	float				_startScale = 10.f;
	float				_endScale = 5.f;
};
