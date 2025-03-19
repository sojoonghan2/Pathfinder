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
        if(GetAnimator()) GetAnimator()->Stop();
        _init = true;
    }

    // TODO: INPUT U 대신 Generator의 hp가 0이 되면 아래의 코드를 실행하도록 수정 필요
    if (INPUT->GetButtonDown(KEY_TYPE::U))
    {
        if (!_isPlaying)
        {
            if(GetAnimator()) GetAnimator()->Play(0);
            _isPlaying = true;
            _deadTime = 0.f;
        }
        auto particle_system = GetGameObject()->GetParticleSystem();
        if (particle_system) particle_system->ParticleStart();
    }
    
    if (_isPlaying)
    {
        _deadTime += 1.f;
    }

    // 애니메이션이 끝났으면 렌더링 종료(인데 아직은 함수 호출이 안됨)
    if (_deadTime > 420.f)
    {
        GetGameObject()->SetRenderOff();
        _isPlaying = false;
    }
}
