#include "pch.h"
#include "FactoryMidScript.h"
#include "Input.h"
#include "Timer.h"
#include "Amature.h"
#include "MeshData.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "ParticleSystem.h"
#include "BaseCollider.h"

void FactoryMidScript::Start()
{
	_particlePool.Init(_particleObjects);
}

void FactoryMidScript::LateUpdate()
{
	float deltaAngle = _rotationSpeed * DELTA_TIME;

	auto rotation = GetTransform()->GetLocalRotation();
	rotation.y += deltaAngle;
	GetTransform()->SetLocalRotation(rotation);
}

void FactoryMidScript::CheckBulletHits(shared_ptr<GameObject> bullet)
{
	bullet->GetCollider()->SetEnable(false);
	auto obj = _particlePool.GetNext();
	if (obj)
	{
		obj->GetParticleSystem()->ParticleStart();
	}
}