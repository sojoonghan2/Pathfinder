#pragma once
#include "Component.h"

class Material;
class Mesh;
class StructuredBuffer;
class Texture;

struct ParticleInfo
{
	// ���� ��ǥ
	Vec3	worldPos;
	// ���� ��� �ð�
	float	curTime;
	// �̵� ����
	Vec3	worldDir;
	// ����
	float	lifeTime;
	// Ȱ�� ����
	int32	alive;
	// �е�(12 + 4 + 12 + 4 + 4 + 12(�е�))
	int32	padding[3];
};

// ��ǻƮ ���̴����� ���� �����͸� �����ϴ� ����
struct ComputeSharedInfo
{
	int32 addCount;
	int32 padding[3];
};

class ParticleSystem : public Component
{
public:
	ParticleSystem(bool refraction);
	virtual ~ParticleSystem();

public:
	virtual void FinalUpdate();
	void Render();
	// ��ƼŬ ���� ����, ���� �ð�, �ּ� ����, �ִ� ����, �ּ� �ӵ�, �ִ� �ӵ�, ���� ũ��, ���� ũ��
	void SetParticleInfo(float createInterval, float accTime, float minLifeTime, float maxLifeTime,
		float minSpeed, float maxSpeed, float startScale, float endScale);

	void SetParticleInterval(float createInterval, float accTime);
	void SetParticleLiftTime(float minLifeTime, float maxLifeTime);
	void SetParticleSpeed(float minSpeed, float maxSpeed);
	void SetParticleScale(float startScale, float endScale);

	void SetParticleTexture(shared_ptr<Texture> texture);

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

	// ��ƼŬ ���� ����
	float				_createInterval = 0.005f;
	// ���� �ð�
	float				_accTime = 0.f;

	// �ּ� ����
	float				_minLifeTime = 0.5f;
	// �ִ� ����
	float				_maxLifeTime = 1.f;

	// �ּ� �ӵ�
	float				_minSpeed = 100;
	// �ִ� �ӵ�
	float				_maxSpeed = 50;

	// ���� ũ��
	float				_startScale = 10.f;
	// ���� ũ��
	float				_endScale = 5.f;
};
