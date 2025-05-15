#include "pch.h"
#include "TestScript.h"
#include "Input.h"
#include "GameFramework.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Timer.h"

TestScript::TestScript() {}

TestScript::~TestScript() {}

void TestScript::LateUpdate()
{
    auto pos = GetTransform()->GetLocalRotation();

    if (INPUT->GetButton(KEY_TYPE::Z))
    {
        pos.x += 0.01;
        cout << "xPos: " << pos.x << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::X))
    {
        pos.x -= 0.01;
        cout << "xPos: " << pos.x << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::C))
    {
        pos.y += 0.01;
        cout << "yPos: " << pos.y << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::V))
    {
        pos.y -= 0.01;
        cout << "yPos: " << pos.y << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::B))
    {
        pos.z += 0.01;
        cout << "zPos: " << pos.z << "\n";
    }
    if (INPUT->GetButton(KEY_TYPE::N))
    {
        pos.z -= 0.01;
        cout << "zPos: " << pos.z << "\n";
    }
    GetTransform()->SetLocalRotation(pos);
}