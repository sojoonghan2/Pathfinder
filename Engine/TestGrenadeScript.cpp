#include "pch.h"
#include "TestGrenadeScript.h"
#include "Input.h"
#include "Engine.h"
#include "Timer.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "ParticleSystem.h"
#include "Transform.h"

TestGrenadeScript::TestGrenadeScript()
    : _throwDelay(0.0f), _pendingThrow(false), _parentTransform(nullptr)
{
}

TestGrenadeScript::~TestGrenadeScript()
{
}

void TestGrenadeScript::LateUpdate()
{
    KeyboardInput();
    MouseInput();

    ThrowGrenade();

    if (_resetThrow)
    {
        _isThrown = true;
        _resetThrow = false;
    }
}

void TestGrenadeScript::KeyboardInput()
{
    if (INPUT->GetButtonDown(KEY_TYPE::E) && !_pendingThrow && !_isThrown)
    {
        GetGameObject()->SetRenderOff();

        // ��ô ��� ���� ����
        _pendingThrow = true;
        _throwDelay = 0.0f;

        // �θ� transform ����
        GetTransform()->RestoreParent();
        auto parent = GetTransform()->GetParent().lock();
        _parentTransform = parent->GetTransform();

        // ��ġ �ʱ�ȭ
        Vec3 parentPos = _parentTransform->GetLocalPosition();
        parentPos.y += 300.f;
        GetTransform()->SetLocalPosition(parentPos);

        // �ӵ� �ʱ�ȭ
        _velocity = Vec3(0, 0, 0);

        // �θ�κ��� �и�
        GetTransform()->RemoveParent();

        // ��ƼŬ ����
        auto particle_system = GetGameObject()->GetParticleSystem();
        if (particle_system)
            particle_system->ParticleStop();

        // ���� �ð� �ʱ�ȭ
        _timeSinceLanded = -1.0f;
    }
}

void TestGrenadeScript::MouseInput()
{
    // ���콺 �Է� �ʿ�� ����
}

void TestGrenadeScript::ThrowGrenade()
{
    if (_pendingThrow)
    {
        _throwDelay += DELTA_TIME;

        if (_throwDelay >= 1.8f)
        {
            _pendingThrow = false;
            _throwDelay = 0.0f;

            if (!_parentTransform)
                return;

            // ��ô ���� ���
            float radian = ToRadian(_angle);

            // �θ� ���� ���� ����
            Vec3 forward = _parentTransform->GetLook();
            forward.y = 0.0f;
            forward.Normalize();

            _velocity = forward * (cos(radian) * _power);
            _velocity.y = sin(radian) * _power;

            _gravity = -9.81f * 200.0f;
            _resetThrow = true;
        }
    }

    if (_isThrown)
    {
        GetGameObject()->SetRenderOn();
        Vec3 pos = GetTransform()->GetLocalPosition();

        _velocity.y += _gravity * DELTA_TIME;
        pos += _velocity * DELTA_TIME;

        GetTransform()->SetLocalPosition(pos);

        if (pos.y <= 0.0f)
        {
            pos.y = 0.0f;
            _isThrown = false;

            auto particle_system = GetGameObject()->GetParticleSystem();
            if (particle_system)
                particle_system->ParticleStart();

            if (_timeSinceLanded < 0.0f)
                _timeSinceLanded = 0.0f;
        }
    }

    if (_timeSinceLanded >= 0.0f)
    {
        _timeSinceLanded += DELTA_TIME;

        if (_timeSinceLanded >= 5.0f)
        {
            GetGameObject()->SetRenderOff();
            _timeSinceLanded = -1.0f;
        }
    }
}
