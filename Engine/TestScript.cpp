#include "pch.h"
#include "TestScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Timer.h"

TestScript::TestScript() {}

TestScript::~TestScript() {}

void TestScript::LateUpdate()
{
    auto pos = GetTransform()->GetLocalPosition();

    if (INPUT->GetButton(KEY_TYPE::Z))
    {
        pos.x += 1.f;
        cout << "xPos: " << pos.x << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::X))
    {
        pos.x -= 1.f;
        cout << "xPos: " << pos.x << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::C))
    {
        pos.y += 1.f;
        cout << "yPos: " << pos.y << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::V))
    {
        pos.y -= 1.f;
        cout << "yPos: " << pos.y << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::B))
    {
        pos.z += 1.f;
        cout << "zPos: " << pos.z << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::N))
    {
        pos.z -= 1.f;
        cout << "zPos: " << pos.z << "\n";
    }
    GetTransform()->SetLocalPosition(pos);
}