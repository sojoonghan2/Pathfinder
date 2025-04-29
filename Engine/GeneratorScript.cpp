#include "pch.h"
#include "GeneratorScript.h"
#include "Input.h"
#include "Animator.h"
#include "GameObject.h"
#include "IceParticleSystem.h"
#include "Timer.h"

GeneratorScript::GeneratorScript() : _isPlaying(false), _init(false)
{
}

void GeneratorScript::LateUpdate()
{
	if (!_init)
	{
		if (GetAnimator()) GetAnimator()->Stop();
		_init = true;
	}

	// TODO: INPUT U ��� Generator�� hp�� 0�� �Ǹ� �Ʒ��� �ڵ带 �����ϵ��� ���� �ʿ�
	if (INPUT->GetButtonDown(KEY_TYPE::U))
	{
		if (!_isPlaying)
		{
			if (GetAnimator()) GetAnimator()->Play(0);
			_isPlaying = true;
			_deadTime = 0.f;
		}
		auto particle_system = GetGameObject()->GetParticleSystem();
		if (particle_system) particle_system->ParticleStart();
	}

	if (_isPlaying)
	{
		_deadTime += DELTA_TIME;
	}

	// �ִϸ��̼��� �������� ������ ����
	if (_deadTime > 7.f)
	{
		GetGameObject()->SetRender(false);
		_isPlaying = false;
	}
}