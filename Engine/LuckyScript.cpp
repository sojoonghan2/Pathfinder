#include "pch.h"
#include "LuckyScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"

LuckyScript::LuckyScript()
{
    
}

LuckyScript::~LuckyScript() {}

void LuckyScript::LateUpdate()
{
    _treasureChest = GET_SINGLE(SceneManager)->FindObjectByName(L"TreasureChest1");

    KeyboardInput();
    MouseInput();

    // 게임 윈도우가 포커스 되었을 때만 입력 처리
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd)
    {
        if (INPUT->GetButton(KEY_TYPE::SUBTRACT)) {
            ApplyModuleAbillities();
            GET_SINGLE(SceneManager)->LoadScene(L"BossScene");
        }
    }
}

void LuckyScript::KeyboardInput()
{
    
}

void LuckyScript::MouseInput()
{
    if (INPUT->GetButton(KEY_TYPE::RBUTTON))
    {
        if (_treasureChest && _treasureChest->GetTransform())
        {
            Vec3 currentRotation = _treasureChest->GetTransform()->GetLocalRotation();
            _treasureChest->GetTransform()->SetLocalRotation(currentRotation + Vec3(0.01f, 0.0f, 0.0f));
        }
    }
}