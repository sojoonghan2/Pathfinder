#include "pch.h"
#include "RazerParticleScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "ParticleSystem.h"

RazerParticleScript::RazerParticleScript(shared_ptr<PlayerScript> playerScript)
    : _playerScript(playerScript)
{
}

RazerParticleScript::~RazerParticleScript()
{
}

void RazerParticleScript::LateUpdate()
{
    // ��Ÿ�� Ÿ�̸� ����
    if (_cooldownTimer > 0.f)
        _cooldownTimer -= DELTA_TIME;

    // ��Ÿ���� ������ ���� �Է� ���
    if (INPUT->GetButtonDown(KEY_TYPE::R) &&
        _delayTimer <= 0.f &&
        _cooldownTimer <= 0.f)
    {
        _delayTimer = 3.5f;
    }

    // �߻� ���� Ÿ�̸� ����
    if (_delayTimer > 0.f)
    {
        _delayTimer -= DELTA_TIME;

        // ���� ������ �� ��ƼŬ ����
        if (_delayTimer <= 0.f)
        {
            if (!(_playerScript->GetIsRazer())) return;

            auto particle_system = GetGameObject()->GetParticleSystem();
            if (particle_system)
                particle_system->ParticleStart();

            _cooldownTimer = _cooldownDuration;
            _delayTimer = 0.f;
        }
    }
}
