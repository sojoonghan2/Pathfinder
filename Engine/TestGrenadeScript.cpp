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
    if (_isThrown) ThrowGrenade();
    
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
        // 초기화
        GetTransform()->RestoreParent();
        auto parentPos = GetTransform()->GetParent().lock()->GetTransform()->GetLocalPosition();
        Vec3 pos = parentPos;
        GetTransform()->SetLocalPosition(pos);
        _velocity = Vec3(0, 0, 0);
        GetTransform()->RemoveParent();

        // 파티클 초기화
        auto particle_system = GetGameObject()->GetParticleSystem();
        if (particle_system) particle_system->ParticleStop();

        // 투사각을 라디안으로 변환
        float radian = ToRadian(_angle);

        // 초기 속도 계산
        _velocity.x = 0.0f;
        _velocity.y = sin(radian) * _power;
        _velocity.z = cos(radian) * _power;

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
        if (particle_system) particle_system->ParticleStart();
    }
}