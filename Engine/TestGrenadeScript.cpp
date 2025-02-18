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

    // �� ������ ���
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
        // ��ġ �ʱ�ȭ
        GetTransform()->RestoreParent();
        auto parentPos = GetTransform()->GetParent().lock()->GetTransform()->GetLocalPosition();
        Vec3 pos = parentPos;
        GetTransform()->SetLocalPosition(pos);

        // �ӵ� �ʱ�ȭ
        _velocity = Vec3(0, 0, 0);

        // �θ�κ��� ����
        GetTransform()->RemoveParent();

        // ������ Ȱ��ȭ
        GetGameObject()->SetRenderOn();

        // ���� ����� �� �ð� �ʱ�ȭ
        _timeSinceLanded = -1.0f;

        // ��ƼŬ �ʱ�ȭ
        auto particle_system = GetGameObject()->GetParticleSystem();
        if (particle_system) particle_system->ParticleStop();

        // ���簢�� �������� ��ȯ
        float radian = ToRadian(_angle);

        // �ʱ� �ӵ� ���
        _velocity.x = 0.0f;
        _velocity.y = sin(radian) * _power;
        _velocity.z = cos(radian) * _power;

        // �߷� ���ӵ� ����
        _gravity = -9.81f * 200.0f;

        // �� ������ ��� �� ����
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
        // ���� ��ġ ��������
        Vec3 pos = GetTransform()->GetLocalPosition();

        // �߷� ���� (y�� ����)
        _velocity.y += _gravity * DELTA_TIME;

        // �ӵ��� ������� ��ġ ������Ʈ
        pos += _velocity * DELTA_TIME;

        // ��ġ ����
        GetTransform()->SetLocalPosition(pos);

        // �ٴڿ� ��Ҵ��� üũ
        if (pos.y <= 0.0f)
        {
            pos.y = 0.0f;
            _isThrown = false;
            auto particle_system = GetGameObject()->GetParticleSystem();
            if (particle_system)
            {
                particle_system->ParticleStart();
            }

            // �ٴڿ� ���� �ð� ���
            if (_timeSinceLanded < 0.0f)
            {
                _timeSinceLanded = 0.0f;
            }
        }
    }
    // �ٴڿ� ��������� �ð� ����
    if (_timeSinceLanded >= 0.0f)
    {
        _timeSinceLanded += DELTA_TIME;

        // 5�� ��� �� ������ ��Ȱ��ȭ
        if (_timeSinceLanded >= 5.0f)
        {
            GetGameObject()->SetRenderOff();
            _timeSinceLanded = -1.0f;
        }
    }
}
