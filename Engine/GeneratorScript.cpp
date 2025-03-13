#include "pch.h"
#include "GeneratorScript.h"
#include "Input.h"
#include "Animator.h"
#include "GameObject.h"
#include "IceParticleSystem.h"

GeneratorScript::GeneratorScript() : _isPlaying(false), _init(false)
{
}

void GeneratorScript::LateUpdate()
{
    if (!_init)
    {
        GetAnimator()->Stop();
        _init = true;
    }

    // TODO: INPUT U ��� Generator�� hp�� 0�� �Ǹ� �Ʒ��� �ڵ带 �����ϵ��� ���� �ʿ�
    if (INPUT->GetButtonDown(KEY_TYPE::U))
    {
        if (!_isPlaying)
        {
            GetAnimator()->Play(0);
            _isPlaying = true;
        }
        auto particle_system = GetGameObject()->GetParticleSystem();
        if (particle_system) particle_system->ParticleStart();
    }

    // �ִϸ��̼��� �������� ������ ����(�ε� ������ �Լ� ȣ���� �ȵ�)
    if (_isPlaying && GetAnimator()->IsAnimationFinished())
    {
        GetGameObject()->SetRenderOff();
        _isPlaying = false;
    }
}
