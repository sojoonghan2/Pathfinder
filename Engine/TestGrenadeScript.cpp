#include "pch.h"
#include "TestGrenadeScript.h"
#include "Input.h"
#include "Engine.h"
#include "Timer.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "ParticleSystem.h"
#include "Transform.h"

TestGrenadeScript::TestGrenadeScript() {}

TestGrenadeScript::~TestGrenadeScript() {}

void TestGrenadeScript::LateUpdate()
{
    KeyboardInput();
    MouseInput();

    // 한 프레임 대기
    ThrowGrenade();
    
    if (_resetThrow)
    {
        _isThrown = true;
        _resetThrow = false;
    }
}

void TestGrenadeScript::KeyboardInput()
{
    if (INPUT->GetButton(KEY_TYPE::Y))
    {
        // 위치 초기화
        GetTransform()->RestoreParent();
        auto parent = GetTransform()->GetParent().lock();
        auto parentTransform = parent->GetTransform();
        Vec3 parentPos = parentTransform->GetLocalPosition();
        GetTransform()->SetLocalPosition(parentPos);

        // 속도 초기화
        _velocity = Vec3(0, 0, 0);

        // 부모로부터 독립
        GetTransform()->RemoveParent();

        // 렌더링 활성화
        GetGameObject()->SetRenderOn();

        // 땅에 닿았을 때 시간 초기화
        _timeSinceLanded = -1.0f;

        // 파티클 초기화
        auto particle_system = GetGameObject()->GetParticleSystem();
        if (particle_system) particle_system->ParticleStop();

        // 투사각을 라디안으로 변환
        float radian = ToRadian(_angle);

        // 부모의 전방 벡터 (Look 방향) 가져오기
        Vec3 forward = parentTransform->GetLook();

        // Y축 방향으로 각도 적용한 초기 속도 계산
        Vec3 direction = forward;
        direction.y = 0.0f;
        direction.Normalize();

        _velocity = direction * (cos(radian) * _power);
        _velocity.y = sin(radian) * _power;

        // 중력 가속도 설정
        _gravity = -9.81f * 200.0f;

        // 한 프레임 대기 후 적용
        _resetThrow = true;
    }
}

void TestGrenadeScript::MouseInput()
{

}

void TestGrenadeScript::ThrowGrenade()
{
    if (_isThrown)
    {
        // 현재 위치 가져오기
        Vec3 pos = GetTransform()->GetLocalPosition();

        // 중력 적용 (y축 방향)
        _velocity.y += _gravity * DELTA_TIME;

        // 속도를 기반으로 위치 업데이트
        pos += _velocity * DELTA_TIME;

        // 위치 적용
        GetTransform()->SetLocalPosition(pos);

        // 바닥에 닿았는지 체크
        if (pos.y <= 0.0f)
        {
            pos.y = 0.0f;
            _isThrown = false;
            auto particle_system = GetGameObject()->GetParticleSystem();
            if (particle_system)
            {
                particle_system->ParticleStart();
            }

            // 바닥에 닿은 시간 기록
            if (_timeSinceLanded < 0.0f)
            {
                _timeSinceLanded = 0.0f;
            }
        }
    }
    // 바닥에 닿아있으면 시간 누적
    if (_timeSinceLanded >= 0.0f)
    {
        _timeSinceLanded += DELTA_TIME;

        // 5초 경과 시 렌더링 비활성화
        if (_timeSinceLanded >= 5.0f)
        {
            GetGameObject()->SetRenderOff();
            _timeSinceLanded = -1.0f;
        }
    }
}
