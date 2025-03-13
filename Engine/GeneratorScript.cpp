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

    // TODO: INPUT U 대신 Generator의 hp가 0이 되면 아래의 코드를 실행하도록 수정 필요
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

    // 애니메이션이 끝났으면 렌더링 종료(인데 아직은 함수 호출이 안됨)
    if (_isPlaying && GetAnimator()->IsAnimationFinished())
    {
        GetGameObject()->SetRenderOff();
        _isPlaying = false;
    }
}
