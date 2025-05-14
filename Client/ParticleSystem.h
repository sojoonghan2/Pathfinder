#pragma once
#include "Component.h"
#include "Material.h"

class Material;
class Mesh;
class Buffer;
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
	// �е�(12 + 4 + 12 + 4 + 12(�е�))
	int32	padding[3];
};

// ��ǻƮ ���̴����� ���� �����͸� �����ϴ� ����
struct ComputeSharedInfo
{
	int32 addCount;
	int32 padding[3];
};

// GPU ��� ��ƼŬ �ý��� ������Ʈ
// ��ǻƮ ���̴��� ���� ��ƼŬ �����͸� ����ϰ�, ���ۿ� ����� ������ �������� ��ƼŬ ������ ����
// ���� �ֱ�, ����, �ӵ�, ũ�� �� ��ƼŬ ���� �Ӽ� ���� ����
class ParticleSystem : public Component
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

public:
	virtual void FinalUpdate();
	void Render();
	// ���� ����, ���� �ð�, �ּ� ����, �ִ� ����, �ּ� �ӵ�, �ִ� �ӵ�, ���� ũ��, ���� ũ��
	void SetParticleInfo(float createInterval, float accTime, float minLifeTime, float maxLifeTime,
		float minSpeed, float maxSpeed, float startScale, float endScale);

	// ���� ����, ���� �ð�(�⺻: 0.005f, 0.f)
	void SetParticleInterval(float createInterval, float accTime);
	// �ּ� ����, �ִ� ����(�⺻: 0.5f, 1.f)
	void SetParticleLifeTime(float minLifeTime, float maxLifeTime);
	// �ּ� �ӵ�, �ִ� �ӵ�(�⺻: 100, 50)
	void SetParticleSpeed(float minSpeed, float maxSpeed);
	// ���� ũ��, ���� ũ��(�⺻: 10.f, 5.f)
	void SetParticleScale(float startScale, float endScale);
	// �ؽ���
	void SetParticleTexture(int index, shared_ptr<Texture> texture);
	// ���� �ð�
	void SetDuration(float duration) { _duration = duration; }
	// ���͸���
	void SetMaterial(shared_ptr<Material> material, shared_ptr<Material> computeMaterial);
	// �� ���� �����Ǵ� ��ƼŬ ��
	void SetOnceParticleNum(int num) { _onceParticleNum = num; }
	// ����
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

	// ��ƼŬ Ȱ�� ����
	bool				_isActive = false;
	// ��ü ��� �ð�
	float				_totalTime = 0.0f;
	// ��ƼŬ ���� �ð�(-1�� ����)
	float				_duration = -1.0f;

	// �� ���� �����Ǵ� ��ƼŬ�� ��
	int					_onceParticleNum = 1;

	// ����
	Vec4				_direction = Vec4(0, 0, 0, 0);
};
