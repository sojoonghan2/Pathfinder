#pragma once
#include "Component.h"
#include "Material.h"

class Material;
class Mesh;
class Buffer;
class Texture;

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
	// 패딩(12 + 4 + 12 + 4 + 12(패딩))
	int32	padding[3];
};

// 컴퓨트 셰이더에서 공유 데이터를 저장하는 버퍼
struct ComputeSharedInfo
{
	int32 addCount;
	int32 padding[3];
};

// GPU 기반 파티클 시스템 컴포넌트
// 컴퓨트 셰이더를 통해 파티클 데이터를 계산하고, 버퍼에 저장된 정보를 바탕으로 파티클 렌더링 수행
// 생성 주기, 수명, 속도, 크기 등 파티클 개별 속성 설정 가능
class ParticleSystem : public Component
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

public:
	virtual void FinalUpdate();
	void Render();
	// 생성 간격, 누적 시간, 최소 수명, 최대 수명, 최소 속도, 최대 속도, 시작 크기, 종료 크기
	void SetParticleInfo(float createInterval, float accTime, float minLifeTime, float maxLifeTime,
		float minSpeed, float maxSpeed, float startScale, float endScale);

	// 생성 간걱, 누적 시간(기본: 0.005f, 0.f)
	void SetParticleInterval(float createInterval, float accTime);
	// 최소 수명, 최대 수명(기본: 0.5f, 1.f)
	void SetParticleLifeTime(float minLifeTime, float maxLifeTime);
	// 최소 속도, 최대 속도(기본: 100, 50)
	void SetParticleSpeed(float minSpeed, float maxSpeed);
	// 시작 크기, 종료 크기(기본: 10.f, 5.f)
	void SetParticleScale(float startScale, float endScale);
	// 텍스쳐
	void SetParticleTexture(int index, shared_ptr<Texture> texture);
	// 지속 시간
	void SetDuration(float duration) { _duration = duration; }
	// 머터리얼
	void SetMaterial(shared_ptr<Material> material, shared_ptr<Material> computeMaterial);
	// 한 번에 생성되는 파티클 양
	void SetOnceParticleNum(int num) { _onceParticleNum = num; }
	// 방향
	void SetEmitDirection(const Vec4& direction) { _direction = direction; }

	void ParticleStart();
	void ParticleStop();
	void ParticleToggle();

	bool IsActive() { return _isActive; }

	shared_ptr<Material> GetComputeMaterial() { return _computeMaterial; }

public:
	virtual void Load(const wstring& path) override {}
	virtual void Save(const wstring& path) override {}

private:
	shared_ptr<Buffer>	_particleBuffer;
	shared_ptr<Buffer>	_computeSharedBuffer;
	uint32							_maxParticle = 10000;

	shared_ptr<Material>		_computeMaterial;
	shared_ptr<Material>		_material;
	shared_ptr<Mesh>			_mesh;

	// 파티클 생성 간격
	float				_createInterval = 0.005f;
	// 누적 시간
	float				_accTime = 0.f;

	// 최소 수명
	float				_minLifeTime = 0.5f;
	// 최대 수명
	float				_maxLifeTime = 1.f;

	// 최소 속도
	float				_minSpeed = 100;
	// 최대 속도
	float				_maxSpeed = 50;

	// 시작 크기
	float				_startScale = 10.f;
	// 종료 크기
	float				_endScale = 5.f;

	// 파티클 활성 상태
	bool				_isActive = false;
	// 전체 경과 시간
	float				_totalTime = 0.0f;
	// 파티클 지속 시간(-1은 무한)
	float				_duration = -1.0f;

	// 한 번에 생성되는 파티클의 양
	int					_onceParticleNum = 1;

	// 방향
	Vec4				_direction = Vec4(0, 0, 0, 0);
};
