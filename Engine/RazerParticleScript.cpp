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
    // 쿨타임 타이머 감소
    if (_cooldownTimer > 0.f)
        _cooldownTimer -= DELTA_TIME;

    // 쿨타임이 끝났을 때만 입력 허용
    if (INPUT->GetButtonDown(KEY_TYPE::R) &&
        _delayTimer <= 0.f &&
        _cooldownTimer <= 0.f)
    {
        _delayTimer = 3.5f;
    }

    // 발사 지연 타이머 감소
    if (_delayTimer > 0.f)
    {
        _delayTimer -= DELTA_TIME;

        // 지연 끝났을 때 파티클 실행
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
