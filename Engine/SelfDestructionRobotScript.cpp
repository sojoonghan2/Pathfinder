#include "pch.h"
#include "SelfDestructionRobotScript.h"
#include "Input.h"
#include "Engine.h"
#include "Timer.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "ParticleSystem.h"
#include "Transform.h"

SelfDestructionRobotScript::SelfDestructionRobotScript()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> angleDis(0.0f, 360.0f);

	float angle = angleDis(gen) * (3.141592f / 180.0f);
	_direction.x = cos(angle);
	_direction.z = sin(angle);
	_direction.y = 0.0f;
}

SelfDestructionRobotScript::~SelfDestructionRobotScript() {}

void SelfDestructionRobotScript::LateUpdate()
{
	MoveRandomly();
	CheckBoundary();
}

void SelfDestructionRobotScript::MoveRandomly()
{
	_elapsedTime += DELTA_TIME;

	Vec3 pos = GetTransform()->GetLocalPosition();
	pos += _direction * _speed * DELTA_TIME;
	GetTransform()->SetLocalPosition(pos);
}

void SelfDestructionRobotScript::CheckBoundary()
{
	Vec3 pos = GetTransform()->GetLocalPosition();

	float mapMinX = -4900.f;
	float mapMaxX = 4900.f;
	float mapMinZ = -4900.f;
	float mapMaxZ = 4900.f;

	if (pos.x <= mapMinX || pos.x >= mapMaxX)
	{
		_direction.x *= -1;
		pos.x = max(mapMinX, min(pos.x, mapMaxX));
	}

	if (pos.z <= mapMinZ || pos.z >= mapMaxZ)
	{
		_direction.z *= -1;
		pos.z = max(mapMinZ, min(pos.z, mapMaxZ));
	}

	GetTransform()->SetLocalPosition(pos);
}